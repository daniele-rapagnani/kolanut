#pragma once

extern "C" {
#include <melon/core/vm.h>
#include <melon/core/tstring.h>
#include <melon/modules/modules.h>
}

#include <string>

namespace kola {
namespace melon {
namespace ffi {

inline Value* getModule(VM& vm, const std::string& module)
{
    Value moduleStrVal;
    moduleStrVal.type = MELON_TYPE_STRING;
    moduleStrVal.pack.obj = melNewString(&vm, module.c_str(), module.size());
    melM_stackPush(&vm.stack, &moduleStrVal);
    Value* val = melGetValueFromGlobalModule(&vm, &moduleStrVal);
    melM_stackPop(&vm.stack);

    return val;
}

inline Value* getValueFromModule(
    VM& vm, 
    TType type, 
    const std::string& module, 
    const std::string& key
)
{
    if (!module.empty())
    {
        GCItem* moduleStr = melNewString(&vm, module.c_str(), module.size());
        melM_vstackPushGCItem(&vm.stack, moduleStr);
    }
    else
    {
        melM_vstackPushNull(&vm.stack);
    }
    

    GCItem* keyStrs = melNewString(&vm, key.c_str(), key.size());
    melM_vstackPushGCItem(&vm.stack, keyStrs);

    Value* val = melGetTypeFromModule(
        &vm, 
        melM_stackOffset(&vm.stack, 1), 
        melM_stackTop(&vm.stack), 
        type
    );

    melM_stackPopCount(&vm.stack, 2);

    return val;
}

inline void newSubmodule(
    VM* vm,
    const std::string& parentModule, 
    const std::string& name,
    const ModuleFunction* funcs
)
{
    Value* val = ffi::getModule(*vm, parentModule);
    assert(val);

    Value key;
    key.type = MELON_TYPE_STRING;
    key.pack.obj = melNewString(vm, name.c_str(), name.size());
    melM_stackPush(&vm->stack, &key);

    if (melNewModule(vm, funcs) != 0)
    {
        knM_logError("Can't create module for Texture");
        return;
    }

    Value* mod = melM_stackTop(&vm->stack);
    melSetValueObject(vm, val->pack.obj, &key, mod);

    // Pop key and module
    melM_stackPopCount(&vm->stack, 2);
}

} // namespace ffi
} // namespace melon
} // namespace kola