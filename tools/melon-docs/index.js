const glob = require("glob");
const path = require("path");
const fs = require("fs");
const nunjucks = require("nunjucks");

const argv = require("yargs")
    .option(
        "modules-dir", {
            alias: "m",
            default: path.join(__dirname, "..", "..", "src", "kolanut", "scripting", "melon", "modules")
        }
    )
    .option(
        "modules-sources-dir", {
            alias: "s",
            default: path.join(__dirname, "..", "..", "src", "kolanut", "scripting", "melon", "modules", "sources")
        }
    )
    .option(
        "bindings-dir", {
            alias: "b",
            default: path.join(__dirname, "..", "..", "src", "kolanut", "scripting", "melon", "bindings")
        }
    )
    .option(
        "bindings-glob", {
            alias: "B",
            default: "*.cpp"
        }
    )
    .option(
        "name-format", {
            alias: "f",
            default: "{}.md"
        }
    )
    .option(
        "module-glob", {
            alias: "g",
            default: "*Module.cpp"
        }
    )
    .option(
        "module-sources-glob", {
            alias: "G",
            default: "*.ms"
        }
    )
    .option(
        "index-file-template", {
            alias: "I",
            default: path.join(__dirname, "index.md")
        }
    )
    .option(
        "index-file", {
            alias: "i",
            default: path.join(__dirname, "..", "..", "src", "index.md")
        }
    )
    .option(
        "template", {
            alias: "t",
            default: path.join(__dirname, "template.md")
        }
    )
    .option(
        "docs-url-base", {
            alias: "u",
            default: "https://github.com/daniele-rapagnani/kolanut/tree/master"
        }
    )
    .argv
;

const escapeRegExp = (string) =>
    string.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
;

const removeCommentAsterisks = (lines) => 
    lines.map((l) => l.replace(/^\s+\*\s+/m, "").trim())
    .filter((l) => l.length > 0)
;

const docsFromMatch = (match) => 
    removeCommentAsterisks(match.split("\n")).join("\n")
;

const getModuleDescription = (source, modPath) => {
    const re = /\/\*\*\*(.*?@module.*?)\*\//s;
    let match = re.exec(source);

    if (match === null) {
        console.warn(`${path.basename(modPath)} is missing a @module section`);
        return null;
    }

    const moduleDesc = {
        exports: []
    };

    let docs = docsFromMatch(match[1]);
    const exportsRe = /^@exports\s+([\w\.]+)\s+(.*)$/gm;

    let reMatch = null;

    while ((reMatch = exportsRe.exec(docs)) !== null) {
        moduleDesc.exports.push({
            name: reMatch[1],
            description: reMatch[2]
        });
    }

    docs = docs.replace(/@module/g, "");
    docs = docs.replace(exportsRe, "");

    moduleDesc.summary = docs;

    return moduleDesc;
};

const getCustomModuleFunctions = (source, functions) => {
    const re = /\/\*\*\*(?:(?:(?!\*\/).)*?)@funcdef\s+(\w+).*?\*\//gs;
    let funcMatch = null;

    while ((funcMatch = re.exec(source)) !== null) {
        functions.push({
            name: funcMatch[1].trim(),
            argsCount: null,
            realFuncName: null
        });
    };
};

const MELON_OPERATORS = {
    "object.symbols.sumOperator": "operator +",
    "object.symbols.subOperator": "operator -",
    "object.symbols.mulOperator": "operator *",
    "object.symbols.divOperator": "operator /",
    "object.symbols.concatOperator": "operator ..",
    "object.symbols.compareOperator": "operator <>",
    "object.symbols.getIndexOperator": "operator []",
    "object.symbols.setIndexOperator": "operator [] =",
    "object.symbols.negOperator": "operator unary -",
    "object.symbols.sizeOperator": "operator #",
    "object.symbols.powOperator": "operator ^",
    "object.symbols.callOperator": "operator ()",
    "object.symbols.hashingFunction": "hashing",
    "object.symbols.iterator": "operator unary * (iterator)",
    "object.symbols.nextFunction": "operator unary > (next item)",
    "object.symbols.getPropertyOperator": "operator .",
    "object.symbols.setPropertyOperator": "operator .=",
};

const C_MELON_OPERATORS = {
    "MELON_OBJSYM_ADD": "operator +",
    "MELON_OBJSYM_SUB": "operator -",
    "MELON_OBJSYM_MUL": "operator *",
    "MELON_OBJSYM_DIV": "operator /",
    "MELON_OBJSYM_CONCAT": "operator ..",
    "MELON_OBJSYM_CMP": "operator <>",
    "MELON_OBJSYM_INDEX": "operator []",
    "MELON_OBJSYM_SETINDEX": "operator [] =",
    "MELON_OBJSYM_NEG": "operator unary -",
    "MELON_OBJSYM_SIZEARR": "operator #",
    "MELON_OBJSYM_POW": "operator ^",
    "MELON_OBJSYM_CALL": "operator ()",
    "MELON_OBJSYM_HASH": "hashing",
    "MELON_OBJSYM_ITERATOR": "operator unary * (iterator)",
    "MELON_OBJSYM_NEXT": "operator unary > (next item)",
    "MELON_OBJSYM_PROPERTY": "operator .",
    "MELON_OBJSYM_SETPROPERTY": "operator .=",
};

const getMelonClassFunctions = (source) => {
    const re = /\s*\[?([a-zA-Z_][a-zA-Z_0-9.]*)\]?\s*=\s*(?:func\s+[a-zA-Z_][a-zA-Z_0-9]*\s+)?(\|.*?\|)?\s*((?:-|=)>)/gs;
    let reMatch = null;
    let functions = [];

    while ((reMatch = re.exec(source)) !== null) {
        const name = reMatch[1];
        const args = (reMatch[2] || "||").replace(/\|/g, "").split(",").map((arg) => arg.trim());

        type = (reMatch[3] || "=>") == "->" ? "method" : "function";

        if (MELON_OPERATORS[name]) {
            type = "operator";
        }

        functions.push({
            name,
            argsCount: args.length,
            realFuncName: type == "operator" ? `[${name}]` : name,
            type,
            operator: MELON_OPERATORS[name],
            docs: {
                args: args.map((a) => ({
                    name: a
                }))
            }
        });
    }

    getCustomModuleFunctions(source, functions);

    return functions;
};

const getModuleFunctions = (source) => {
    const re = /static\s+const\s+ModuleFunction\s+\w+\[\]\s+=\s+\{(.*?)\};/gs;
    const matches = re.exec(source);
    const functions = [];

    if (matches && matches.length >= 2) {
        const functionsDefs = matches[1];
    
        const re2 = /\s*\{(.*?)\}\s*,?\s*/g;
    
        let funcMatch = null;
    
        while ((funcMatch = re2.exec(functionsDefs)) !== null) {
            const parts = funcMatch[1].split(",").map((i) => i.trim());
            let name = parts[0].replace(/"/g, "");
            
            if (name == "NULL") {
                name = "";
            }
    
            const argsCount = parseInt(parts[1]);
            const realFuncName = parts[3].replace("&", "");

            if (realFuncName == "NULL") {
                continue;
            }

            let funcObj = {
                name,
                type: "function",
                argsCount,
                realFuncName
            };

            if (parts.length > 6 && C_MELON_OPERATORS[parts[6]]) {
                funcObj.type = "operator";
                funcObj.operator = C_MELON_OPERATORS[parts[6]];
            } else if (parts.length > 4) {
                if (parseInt(parts[4]) == 1) {
                    funcObj.type = "method";
                }
            }
    
            functions.push(funcObj);
        }
    }

    getCustomModuleFunctions(source, functions);

    return functions;
};

const getCustomFunctionDocs = (source, f) => {
    const re = new RegExp("\\/\\*\\*\\*((?:(?:(?!\\*\\/).)*?)@(function|method|operator)\\s+" + f.name + ".*?)\\*\\/", "s");
    const match = re.exec(source);

    if (match === null) {
        return null;
    }

    f.type = match[2].toLowerCase();

    return docsFromMatch(match[1]).replace(new RegExp("@(function|method|operator)\\s+" + f.name, "sg"), "");
};

const getFunctionDocs = (source, f, re) => {
    const match = re.exec(source);

    let docs = null;

    if (match === null) {
        docs = getCustomFunctionDocs(source, f);

        if (docs === null) {
            console.warn(`Missing documentation for ${f.name}`);
            return null;
        }
    } else {
        docs = docsFromMatch(match[1]);
    }

    const docObj = {
        args: []
    };

    const argsRe = /^@arg\s+([\?\.\w]+)\s+(.*)$/gm;

    let reMatch = null;

    while ((reMatch = argsRe.exec(docs)) !== null) {
        docObj.args.push({
            name: reMatch[1].replace("?", ""),
            optional: reMatch[1][0] == "?",
            description: reMatch[2]
        });
    }

    docs = docs.replace(argsRe, "");

    const returnRe = /^@returns\s+(.*)$/m;

    reMatch = returnRe.exec(docs);

    if (reMatch !== null) {
        docObj.returns = {
            description: reMatch[1]
        }

        docs = docs.replace(returnRe, "");
    }

    docObj.summary = docs;
    f.docs = docObj;

    f.signature = `${f.name}(${docObj.args.map((a) => {
        if (a.optional) {
            return `[${a.name}]`;
        }

        return a.name;
    }).join(", ")})`;
};

const writeModuleDocs = (templatePath, modules, nameFormat, urlBase) => {
    modules.forEach((mod) => {
        const renderedDocs = nunjucks.render(templatePath, mod);
        const fileName = nameFormat.replace("{}", mod.name);
        const fullPath = path.join(mod.outputDir, fileName);

        mod.docUrl = urlBase
            + mod.outputDir.replace(path.resolve(path.join(__dirname, "..", "..")), "")
        ;

        console.log(`Generating docs for ${mod.name} in ${fullPath}`);
        fs.writeFileSync(fullPath, renderedDocs, "utf8");
    });
};

const writeModulesIndex = (modules, template, outfile) => {
    const renderedDocs = nunjucks.render(template, {
        modules
    });

    console.log(`Generating index in ${outfile}`);

    fs.writeFileSync(
        outfile,
        renderedDocs, 
        "utf8"
    );
};

const modules = [];

const processModules = (modulesGlob, functionReBuilder, getFuncs, ignore) => {
    glob.sync(modulesGlob).map(
        (modulePath) => {
            ignore = ignore || {};
            const modName = path.parse(modulePath).name;

            if (ignore[modName]) {
                return;
            }
            
            const modObj = {
                name: modName,
                id: modName.toLowerCase(),
                outputDir: path.dirname(modulePath)
            };
    
            const source = fs.readFileSync(modulePath, "utf8");
            modObj.description = getModuleDescription(source, modulePath);

            if (!modObj.description) {
                return;
            }

            let functions = getFuncs(source);
    
            if (functions) {
                for (const f of functions) {
                    let re = new RegExp(
                        "\\/\\*\\*\\*((?:(?!\\*\\/).)*?)\\*\\/[\\s\\n]*" + functionReBuilder(f),
                        "gs"
                    );

                    getFunctionDocs(source, f, re);
                }
            }
    
            modObj.functions = functions.filter((f) => f.type == "function");
            modObj.methods = functions.filter((f) => f.type == "method");
            modObj.operators = functions.filter((f) => f.type == "operator");
    
            modules.push(modObj);
        }
    );
}

processModules(
    path.join(argv.modulesSourcesDir, argv.moduleSourcesGlob),
    (f) => escapeRegExp(f.realFuncName) + "\\s*=\\s*(?:func\\s+[a-zA-Z_][a-zA-Z_0-9]*\\s+)?(\\|.*?\\|)?\\s*((?:-|=)>)",
    getMelonClassFunctions
);

const ignoreWrappers = modules.reduce((obj, m) => {
    let name = m.name;
    
    if (m.name.length > 1) {
        name = `${m.name.toUpperCase()[0] + m.name.toLowerCase().substr(1)}`;
    }

    obj[`${name}Module`] = true; 
    return obj; 
}, {});

processModules(
    path.join(argv.modulesDir, argv.moduleGlob),
    (f) => "static\\s+TByte\\s+" + escapeRegExp(f.realFuncName) + "\\s*",
    getModuleFunctions,
    ignoreWrappers
);

processModules(
    path.join(argv.bindingsDir, argv.bindingsGlob),
    (f) => "static\\s+TByte\\s+" + escapeRegExp(f.realFuncName) + "\\s*",
    getModuleFunctions,
    ignoreWrappers
);

writeModuleDocs(argv.template, modules, argv.nameFormat, argv.docsUrlBase);
writeModulesIndex(modules, argv.indexFileTemplate, argv.indexFile);