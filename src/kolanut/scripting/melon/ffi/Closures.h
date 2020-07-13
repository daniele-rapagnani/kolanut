#pragma once

#include "kolanut/scripting/melon/ffi/Modules.h"
#include "kolanut/scripting/melon/ffi/PopPrimitives.h"

extern "C" {
#include <melon/core/vm.h>
#include <melon/core/tstring.h>
#include <melon/modules/modules.h>
}

#include <string>

namespace kola {
namespace melon {
namespace ffi {

template <typename ...Types>
void callClosure(VM& vm, Value* closure, TUint16 expRet, Types ...args)
{
    assert(closure->type == MELON_TYPE_CLOSURE);
    
    melM_stackPush(&vm.stack, closure);
    push(&vm, args...);
    melCallClosureSyncVM(&vm, sizeof...(Types), 0, expRet);
}

template <int ExpRet = 0, typename ...Types>
void callModuleClosure(VM& vm, const std::string& module, const std::string& key, Types ...args)
{
    Value* cl = getValueFromModule(
        vm, 
        MELON_TYPE_CLOSURE,
        module,
        key
    );

    if (!cl)
    {
        knM_logError("Can't find function '" << key << "' in module '" << module << "'");
        return;
    }

    callClosure(vm, cl, ExpRet, args...);
}

template <typename TRet, typename ...Types>
bool callModuleClosureRet(VM& vm, const std::string& module, const std::string& key, TRet& res, Types ...args)
{
    callModuleClosure<1>(vm, module, key, args...);
    return pop<TRet>(&vm, res);
}

} // namespace ffi
} // namespace melon
} // namespace kola