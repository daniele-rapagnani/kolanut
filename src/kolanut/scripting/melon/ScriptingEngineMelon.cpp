#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/scripting/melon/ScriptingEngineMelon.h"
#include "kolanut/scripting/melon/modules/KolanutModule.h"
#include "kolanut/scripting/melon/modules/Vector2Module.h"
#include "kolanut/scripting/melon/modules/Vector3Module.h"
#include "kolanut/scripting/melon/modules/Vector4Module.h"
#include "kolanut/scripting/melon/modules/RectModule.h"
#include "kolanut/scripting/melon/modules/ColorModule.h"
#include "kolanut/scripting/melon/modules/BoundingboxModule.h"
#include "kolanut/scripting/melon/ffi/FFI.h"
#include "kolanut/scripting/melon/ffi/Modules.h"
#include "kolanut/scripting/melon/ffi/PopVectors.h"
#include "kolanut/scripting/melon/bindings/Bindings.h"

#include "kolanut/filesystem/FilesystemEngine.h"

extern "C" {
#include <melon/core/tstring.h>
#include <melon/core/closure.h>
#include <melon/core/function.h>
#include <melon/core/compiler.h>
#include <melon/core/vm.h>
#include <melon/modules/modules.h>
}

#include <cassert>

namespace kola {

namespace melon {
namespace ffi {

template <>
inline bool convert(VM* vm, Kolanut::Config& res, Value* val)
{
    if (val->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    getInstanceEnumField<graphics::Engine>(
        vm, 
        val->pack.obj,
        "graphicBackend", 
        res.graphics.renderer, 
        graphics::engineFromString
    );

    getInstanceEnumField<events::Engine>(
        vm, 
        val->pack.obj, 
        "eventSystem", 
        res.events.eventSystem, 
        events::engineFromString
    );

    getInstanceEnumField<graphics::ResolutionFitMode>(
        vm, 
        val->pack.obj,
        "resolutionFitMode",
        res.graphics.resolution.resizeMode,
        graphics::resolutionFitModeFromString
    );

    getInstanceField(vm, val->pack.obj, "windowTitle", res.graphics.windowTitle);
    getInstanceField(vm, val->pack.obj, "screenSize", res.graphics.resolution.screenSize);
    getInstanceField(vm, val->pack.obj, "designResolution", res.graphics.resolution.designResolution);
    getInstanceField(vm, val->pack.obj, "fullscreen", res.graphics.resolution.fullScreen);
    getInstanceField(vm, val->pack.obj, "vsync", res.graphics.resolution.vSynced);
    getInstanceField(vm, val->pack.obj, "enableGraphicAPIDebug", res.graphics.enableAPIDebug);
    getInstanceField(vm, val->pack.obj, "framesInFlight", res.graphics.framesInFlight);
    getInstanceField(vm, val->pack.obj, "maxGeometryBufferVertices", res.graphics.maxGeometryBufferVertices);
    getInstanceField(vm, val->pack.obj, "jobQueueInitialSize", res.graphics.jobQueueInitialSize);
    getInstanceField(vm, val->pack.obj, "forceGPU", res.graphics.forceGPU);
    getInstanceField(vm, val->pack.obj, "enableStats", res.enableStats);

    return true;
}

template <>
class Push<stats::StatsEngine::Result>
{
public:
    static TByte push(VM* vm, const stats::StatsEngine::Result& val)
    {
        GCItem* obj = melNewObject(vm);
        melM_vstackPushGCItem(&vm->stack, obj);

        setInstanceField(vm, obj, "label", *val.label);
        setInstanceField(vm, obj, "avg", val.avg);
        setInstanceField(vm, obj, "total", val.total);
        setInstanceField(vm, obj, "samplesCount", val.samplesCount);

        return 1;
    }
};

} // namespace ffi
} // namespace melon

namespace scripting {

namespace {

Value fileDescSymbol;

Module KOLANUT_MODULES[] = {
    { "Vector2", vector2ModuleInit },
    { "Vector3", vector3ModuleInit },
    { "Vector4", vector4ModuleInit },
    { "BoundingBox", boundingboxModuleInit },
    { "Rect", rectModuleInit },
    { "Color", colorModuleInit },
    { "Kolanut", kolanutModuleInit },
    { NULL, NULL }
};

#include "kolanut/scripting/melon/KeyCodesNames.inl"

} // namespace

bool ScriptingEngineMelon::init(const Config& config)
{
    knM_logDebug("Initilizing Melon scripting");

    this->config = config;

	if (melCreateVM(&this->vm, &this->vmConfig) != 0)
	{
		knM_logFatal("Can't create melon VM");
		return false;
	}

    hookFs();

    if (melRegisterModules(&this->vm, KOLANUT_MODULES) != 0)
    {
        knM_logFatal("Can't init kolanut's melon modules");
        return false;
    }

    melon::bindings::registerBindings(&this->vm);

    return true;
}

bool ScriptingEngineMelon::runScript(const std::string& filename)
{
    TByte* prog = nullptr;
    size_t size;

    std::vector<char> data;

    if (!di::get<filesystem::FilesystemEngine>()->getFileContent(filename, data))
    {
        return false;
    }

    Compiler compiler = {};

    if (
        melCreateCompilerFile(
            &compiler, 
            &this->vm, 
            filename.c_str(), 
            data.data(),
            data.size()
        ) != 0
    )
    {
        knM_logError("Can't create compiler for melon script: " << filename);
        return false;
    }

    if (melRunCompiler(&compiler) != 0)
    {
        knM_logError("Can't compile melon script: " << filename);
        return false;
    }

    if (compiler.hasErrors)
    {
        return false;
    }

    melAddRootGC(&this->vm, &this->vm.gc, compiler.main.func);
    melTriggerGC(&this->vm, &this->vm.gc);

    if (melRunMainFunctionVM(&this->vm, compiler.main.func) != 0)
    {
        knM_logError("Can't run melon script: " << filename);
        return false;
    }

    return true;
}

bool ScriptingEngineMelon::start()
{
    std::string bootScript = getScriptPath(this->config.bootScript);

    if (!runScript(bootScript))
    {
        knM_logFatal("Can't load boot script at: '" << bootScript << "'");
        return false;
    }

    return true;
}

bool ScriptingEngineMelon::loadConfig(Kolanut::Config& config)
{
    return melon::ffi::callModuleClosureRet(
        this->vm, 
        "import", 
        &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_CALL],
        config,
        getScriptPath(this->config.configScript)
    );
}

extern "C" {

static TByte openHook(VM* vm)
{
    melM_arg(vm, path, MELON_TYPE_STRING, 0);
    melM_argOptional(vm, flags, MELON_TYPE_STRING, 1);

    uint32_t mode = filesystem::FilesystemEngine::OpenMode::READ;

    if (flags->type != MELON_TYPE_NULL)
    {
        String* str = melM_strFromObj(flags->pack.obj);

        if (str->len > 0 && str->string[0] == 'w')
        {
            mode = filesystem::FilesystemEngine::OpenMode::WRITE;
        }
        
        if (str->len > 1 && str->string[1] == 'b')
        {
            mode |= filesystem::FilesystemEngine::OpenMode::BINARY;
        }
    }
    
    const void* handle = 
        di::get<filesystem::FilesystemEngine>()->open(melM_strDataFromObj(path->pack.obj), mode)
    ;

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* val = melM_stackAllocRaw(&vm->stack);
    val->type = MELON_TYPE_OBJECT;
    val->pack.obj = melNewObject(vm);

    Value fd = {};
    fd.type = handle ? MELON_TYPE_NATIVEPTR : MELON_TYPE_NULL;
    fd.pack.obj = const_cast<GCItem*>(reinterpret_cast<const GCItem*>(handle));

    melSetValueObject(vm, val->pack.obj, &fileDescSymbol, &fd);

    return 1;
}

static TByte closeHook(VM* vm)
{
    melM_arg(vm, handleObj, MELON_TYPE_OBJECT, 0);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* val = melM_stackAllocRaw(&vm->stack);
    val->type = MELON_TYPE_BOOL;
    val->pack.value.boolean = 0;

    Value* handle = melGetValueObject(vm, handleObj->pack.obj, &fileDescSymbol);

    if (!handle || handle->type != MELON_TYPE_NATIVEPTR)
    {
        return 1;
    }

    di::get<filesystem::FilesystemEngine>()->close(
        reinterpret_cast<const void*>(handle->pack.obj)
    );

    val->pack.value.boolean = 1;

    return 1;
}

static TByte readHook(VM* vm)
{
    melM_arg(vm, fdObj, MELON_TYPE_OBJECT, 0);
    melM_argOptional(vm, sizeVal, MELON_TYPE_INTEGER, 1);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);

    result->type = MELON_TYPE_NULL;

    TSize count = 0;
    melon::ffi::convert(vm, count, sizeVal);

    Value* fd = melGetValueObject(vm, fdObj->pack.obj, &fileDescSymbol);

    if (!fd || fd->type != MELON_TYPE_NATIVEPTR)
    {
        return 1;
    }

    const void* handle = reinterpret_cast<const void*>(fd->pack.obj);

    if (count == 0)
    {
        count = di::get<filesystem::FilesystemEngine>()->getFileSize(handle);
    }

    if (count == 0)
    {
        return 1;
    }

    TSize dataSize = count;

    GCItem* strData = melNewDataString(vm, dataSize + 1);
    char* data = melM_strDataFromObj(strData);

    if (di::get<filesystem::FilesystemEngine>()->read(handle, data, dataSize) == 0)
    {
        return 1;
    }

    data[dataSize] = '\0';

    if (melUpdateStringHash(vm, strData) != 0)
    {
        return 1;
    }

    result->type = strData->type;
    result->pack.obj = strData;

    return 1;
}

static TByte writeHook(VM* vm)
{
    melM_arg(vm, fdObj, MELON_TYPE_OBJECT, 0);
    melM_arg(vm, data, MELON_TYPE_STRING, 1);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = 0;

    Value* fd = melGetValueObject(vm, fdObj->pack.obj, &fileDescSymbol);

    if (!fd || fd->type != MELON_TYPE_NATIVEPTR)
    {
        return 1;
    }

    const void* handle = reinterpret_cast<const void*>(fd->pack.obj);

    String* dataStr = melM_strFromObj(data->pack.obj);

    result->pack.value.boolean = di::get<filesystem::FilesystemEngine>()->write(
        handle,
        dataStr->string,
        dataStr->len
    ) == dataStr->len;

    return 1;
}

static TByte sizeHook(VM* vm)
{
    melM_arg(vm, fdObj, MELON_TYPE_OBJECT, 0);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_NULL;

    Value* fd = melGetValueObject(vm, fdObj->pack.obj, &fileDescSymbol);

    if (!fd || fd->type != MELON_TYPE_NATIVEPTR)
    {
        return 1;
    }

    const void* handle = reinterpret_cast<const void*>(fd->pack.obj);

    result->type = MELON_TYPE_INTEGER;
    result->pack.value.integer = di::get<filesystem::FilesystemEngine>()->getFileSize(handle);

    return 1;
}

static TByte isFileHook(VM* vm)
{
    melM_arg(vm, path, MELON_TYPE_STRING, 0);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = di::get<filesystem::FilesystemEngine>()->isFile(
        melM_strDataFromObj(path->pack.obj)
    );

    return 1;
}

static TByte existsHook(VM* vm)
{
    melM_arg(vm, path, MELON_TYPE_STRING, 0);

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_BOOL;
    result->pack.value.boolean = di::get<filesystem::FilesystemEngine>()->exists(
        melM_strDataFromObj(path->pack.obj)
    );

    return 1;
}

static TByte realPathHook(VM* vm)
{
    melM_arg(vm, path, MELON_TYPE_STRING, 0);
    
    std::string resolved = di::get<filesystem::FilesystemEngine>()->resolvePath(
        melM_strDataFromObj(path->pack.obj)
    );

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* result = melM_stackAllocRaw(&vm->stack);
    result->type = MELON_TYPE_STRING;
    result->pack.obj = melNewString(vm, resolved.c_str(), resolved.size());

    return 1;
}

static TByte emptyHook(VM* vm)
{
    return 0;
}

}

void ScriptingEngineMelon::hookFs()
{
    if (melCreateGlobalSymbolKey(&this->vm, "Kolanut file descriptor", &fileDescSymbol) != 0)
    {
        knM_logFatal("Can't create melon file descriptor symbol key");
        return;
    }

    melon::ffi::hookIntoModuleClosure(
        this->vm, 
        MELON_TYPE_CLOSURE, 
        "io", 
        "open",
        &openHook
    );

    melon::ffi::hookIntoModuleClosure(
        this->vm, 
        MELON_TYPE_CLOSURE, 
        "io", 
        "read",
        &readHook
    );

    melon::ffi::hookIntoModuleClosure(
        this->vm, 
        MELON_TYPE_CLOSURE, 
        "io", 
        "write",
        &writeHook
    );

    melon::ffi::hookIntoModuleClosure(
        this->vm, 
        MELON_TYPE_CLOSURE, 
        "io", 
        "close",
        &closeHook
    );

    melon::ffi::hookIntoModuleClosure(
        this->vm, 
        MELON_TYPE_CLOSURE, 
        "io", 
        "size",
        &sizeHook
    );

    melon::ffi::hookIntoModuleClosure(
        this->vm, 
        MELON_TYPE_CLOSURE, 
        "io", 
        "tell",
        &emptyHook
    );

    melon::ffi::hookIntoModuleClosure(
        this->vm, 
        MELON_TYPE_CLOSURE, 
        "io", 
        "seek",
        &emptyHook
    );

    melon::ffi::hookIntoModuleClosure(
        this->vm, 
        MELON_TYPE_CLOSURE, 
        "io", 
        "flush",
        &emptyHook
    );

    melon::ffi::hookIntoModuleClosure(
        this->vm, 
        MELON_TYPE_CLOSURE, 
        "io", 
        "isEOF",
        &emptyHook
    );

    melon::ffi::hookIntoModuleClosure(
        this->vm, 
        MELON_TYPE_CLOSURE, 
        "fs", 
        "isFile",
        &isFileHook
    );

    melon::ffi::hookIntoModuleClosure(
        this->vm, 
        MELON_TYPE_CLOSURE, 
        "fs", 
        "exists",
        &existsHook
    );

    melon::ffi::hookIntoModuleClosure(
        this->vm, 
        MELON_TYPE_CLOSURE, 
        "path", 
        "realpath",
        &realPathHook
    );
}

void ScriptingEngineMelon::onLoad()
{
    melon::ffi::callModuleClosure(this->vm, "Kolanut", "onLoad");
}

void ScriptingEngineMelon::onUpdate(float dt)
{
    melon::ffi::callModuleClosure(this->vm, "Kolanut", "onUpdate", dt);
}

void ScriptingEngineMelon::onDraw()
{
    melon::ffi::callModuleClosure(this->vm, "Kolanut", "onDraw");
}

void ScriptingEngineMelon::onDrawUI()
{
    melon::ffi::callModuleClosure(this->vm, "Kolanut", "onDrawUI");
}

void ScriptingEngineMelon::onStatsUpdated(const stats::StatsEngine::Result& result)
{
    melon::ffi::callModuleClosure(this->vm, "Kolanut", "onStatsUpdated", result);
}

bool ScriptingEngineMelon::onQuit()
{
    bool res = true;
    melon::ffi::callModuleClosureRet(this->vm, "Kolanut", "onQuit", res);
    return res;
}

void ScriptingEngineMelon::onKeyPressed(events::KeyCode key, bool pressed)
{
    const char* keyName = KEYCODE_NAMES[static_cast<unsigned int>(key)];
    melon::ffi::callModuleClosure(this->vm, "Kolanut", "onKeyPressed", keyName, pressed);
}

} // namespace scripting
} // namespace kola