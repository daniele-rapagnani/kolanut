#pragma once

#include "kolanut/core/Logging.h"
#include "kolanut/scripting/melon/ffi/PushPrimitives.h"
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
std::shared_ptr<T>* getInstancePtr(VM* vm, GCItem* obj)
{
    ensureInstanceSymbol(vm);
    Value* val = melGetValueObject(vm, obj, &instanceSymbol);

    if (!val || val->type != MELON_TYPE_NATIVEPTR)
    {
        return nullptr;
    }

    return reinterpret_cast<std::shared_ptr<T>*>(val->pack.obj);
}

template <typename T>
bool pushInstance(VM* vm, GCItem* prototype, std::shared_ptr<T> nativeInst)
{
    if (!nativeInst)
    {
        melM_vstackPushNull(&vm->stack);
        return true;
    }

    GCItem* inst = melNewObject(vm);
    melM_vstackPushGCItem(&vm->stack, inst);
    melSetPrototypeObject(vm, inst, prototype);

    ensureInstanceSymbol(vm);

    std::shared_ptr<T>* ptr = new std::shared_ptr<T>();
    *ptr = nativeInst;

    Value instVal;
    instVal.type = MELON_TYPE_NATIVEPTR;
    instVal.pack.obj = reinterpret_cast<GCItem*>(ptr);

    melSetValueObject(vm, inst, &instanceSymbol, &instVal);

    Object* obj = melM_objectFromObj(inst);
    obj->freeCb = [] (VM* vm, GCItem* item) {
        std::shared_ptr<T>* ptr = getInstancePtr<T>(vm, item);
        delete ptr;
    };

    return true;
}

template <typename T>
bool pushInstance(VM* vm, const std::string& module, const std::string& className, std::shared_ptr<T> nativeInst)
{
    if (!nativeInst)
    {
        melM_vstackPushNull(&vm->stack);
        return true;
    }

    Value* classObj = getValueFromModule(*vm, MELON_TYPE_OBJECT, module, className);

    if (!classObj || classObj->type != MELON_TYPE_OBJECT)
    {
        knM_logError("Can't find base class '" << className << "'");
        return false;
    }

    return pushInstance(vm, classObj->pack.obj, nativeInst);
}

template <typename ...Types>
bool createInstance(
    VM* vm, 
    const std::string& module, 
    const std::string& className,
    const std::string& constructor,
    Types ...args
)
{
    Value* classObj = getValueFromModule(*vm, MELON_TYPE_OBJECT, module, className);

    if (!classObj)
    {
        knM_logError("Can't find class '" << className << "'");
        return false;
    }

    push(vm, constructor);
    Value* cs = melGetValueObject(vm, classObj->pack.obj, melM_stackTop(&vm->stack));
    melM_stackPop(&vm->stack);

    if (cs->type != MELON_TYPE_CLOSURE)
    {
        knM_logError("Class '" << className << "' has no constructor '" << constructor << "'");
        return false;
    }

    callClosure(*vm, cs, 1, args...);

    return true;
}

template <typename T>
bool getInstanceField(VM* vm, GCItem* obj, const std::string& name, T& res)
{
    if (obj->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    melM_vstackPushGCItem(&vm->stack, melNewString(vm, name.c_str(), name.size()));
    Value* val = melResolveValueObject(vm, obj, melM_stackTop(&vm->stack));
    melM_stackPop(&vm->stack);

    if (!val)
    {
        return false;
    }

    return convert(vm, res, val);
}

template <typename T>
bool setInstanceField(VM* vm, GCItem* obj, const std::string& name, T&& res)
{
    if (obj->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    melM_vstackPushGCItem(&vm->stack, melNewString(vm, name.c_str(), name.size()));

    if (!push(vm, res))
    {
        melM_stackPop(&vm->stack);
        return false;
    }

    melSetValueObject(vm, obj, melM_stackOffset(&vm->stack, 1), melM_stackTop(&vm->stack));
    melM_stackPopCount(&vm->stack, 2);

    return true;
}

template <typename T>
std::shared_ptr<T> getInstance(VM* vm, GCItem* obj)
{
    return *getInstancePtr<T>(vm, obj);
}

} // namespace ffi
} // namespace melon
} // namespace kola