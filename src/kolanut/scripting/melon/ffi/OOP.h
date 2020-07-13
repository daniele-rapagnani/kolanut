#pragma once

#include "kolanut/core/Logging.h"
#include "kolanut/scripting/melon/ffi/Closures.h"

extern "C" {
#include <melon/core/vm.h>
#include <melon/core/tstring.h>
#include <melon/core/symbol.h>
#include <melon/modules/modules.h>
}

#include <string>
#include <unordered_map>
#include <memory>

namespace kola {
namespace melon {
namespace ffi {

extern Value instanceSymbol;

inline void ensureInstanceSymbol(VM* vm)
{
    if (instanceSymbol.type == MELON_TYPE_NONE)
    {
        GCItem* label = melNewString(vm, "Class Instance", strlen("Class Instance"));
        melM_vstackPushGCItem(&vm->stack, label);

        instanceSymbol.type = MELON_TYPE_SYMBOL;
        instanceSymbol.pack.obj = melNewSymbol(vm, label);
        melAddRootGC(vm, &vm->gc, instanceSymbol.pack.obj);
        melM_stackPop(&vm->stack);
    }
}

template <typename T>
bool pushInstance(VM* vm, std::shared_ptr<T> nativeInst, const std::string& module, const std::string& className)
{
    Value* classObj = getValueFromModule(*vm, MELON_TYPE_OBJECT, module, className);

    if (!classObj)
    {
        knM_logError("Can't find base class '" << className << "'");
        return false;
    }

    GCItem* inst = melNewObject(vm);
    melM_vstackPushGCItem(&vm->stack, inst);

    melSetPrototypeObject(vm, inst, classObj->pack.obj);

    ensureInstanceSymbol(vm);

    std::shared_ptr<T>* ptr = new std::shared_ptr<T>();
    *ptr = nativeInst;

    Value instVal;
    instVal.type = MELON_TYPE_NATIVEPTR;
    instVal.pack.obj = reinterpret_cast<GCItem*>(ptr);

    melSetValueObject(vm, inst, &instanceSymbol, &instVal);

    return true;
}

template <typename T>
std::shared_ptr<T> getInstance(VM* vm, GCItem* obj)
{
    ensureInstanceSymbol(vm);
    Value* val = melGetValueObject(vm, obj, &instanceSymbol);

    if (!val || val->type != MELON_TYPE_NATIVEPTR)
    {
        return nullptr;
    }

    return *reinterpret_cast<std::shared_ptr<T>*>(val->pack.obj);
}

} // namespace ffi
} // namespace melon
} // namespace kola