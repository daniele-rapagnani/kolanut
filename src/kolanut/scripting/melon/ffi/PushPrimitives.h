#pragma once

extern "C" {
#include <melon/core/vm.h>
#include <melon/core/tstring.h>
#include <melon/modules/modules.h>
}

#include <cstdint>

namespace kola {
namespace melon {
namespace ffi {

template <typename T>
class Push
{
public:
    static TByte push(VM* vm, const T& val) = delete;
};

template <>
class Push<bool>
{
public:
    static TByte push(VM* vm, const bool& val)
    {
        melM_stackEnsure(&vm->stack, vm->stack.top + 1);
        Value* stackVal = melM_stackAllocRaw(&vm->stack);
        stackVal->type = MELON_TYPE_BOOL;
        stackVal->pack.value.boolean = val ? 1 : 0;

        return 1;
    }
};

template <>
class Push<unsigned long>
{
public:
    static TByte push(VM* vm, const unsigned long& val)
    {
        melM_stackEnsure(&vm->stack, vm->stack.top + 1);
        Value* stackVal = melM_stackAllocRaw(&vm->stack);
        stackVal->type = MELON_TYPE_INTEGER;
        stackVal->pack.value.integer = val;

        return 1;
    }
};

template <>
class Push<unsigned long long>
{
public:
    static TByte push(VM* vm, const unsigned long long& val)
    {
        melM_stackEnsure(&vm->stack, vm->stack.top + 1);
        Value* stackVal = melM_stackAllocRaw(&vm->stack);
        stackVal->type = MELON_TYPE_INTEGER;
        stackVal->pack.value.integer = val;

        return 1;
    }
};

template <>
class Push<long long>
{
public:
    static TByte push(VM* vm, const long long& val)
    {
        melM_stackEnsure(&vm->stack, vm->stack.top + 1);
        Value* stackVal = melM_stackAllocRaw(&vm->stack);
        stackVal->type = MELON_TYPE_INTEGER;
        stackVal->pack.value.integer = val;

        return 1;
    }
};

template <>
class Push<long>
{
public:
    static TByte push(VM* vm, const long& val)
    {
        return Push<long long>::push(vm, val);
    }
};

template <>
class Push<int>
{
public:
    static TByte push(VM* vm, const int& val)
    {
        return Push<long long>::push(vm, val);
    }
};

template <>
class Push<unsigned int>
{
public:
    static TByte push(VM* vm, const unsigned int& val)
    {
        return Push<unsigned long>::push(vm, val);
    }
};

template <>
class Push<double>
{
public:
    static TByte push(VM* vm, const double& val)
    {
        melM_stackEnsure(&vm->stack, vm->stack.top + 1);
        Value* stackVal = melM_stackAllocRaw(&vm->stack);
        stackVal->type = MELON_TYPE_NUMBER;
        stackVal->pack.value.number = val;

        return 1;
    }
};

template <>
class Push<float>
{
public:
    static TByte push(VM* vm, const float& val)
    {
        return Push<double>::push(vm, static_cast<double>(val));
    }
};

template <>
class Push<const char*>
{
public:
    static TByte push(VM* vm, const char* const& val)
    {
        melM_stackEnsure(&vm->stack, vm->stack.top + 1);
        Value* stackVal = melM_stackAllocRaw(&vm->stack);
        stackVal->type = MELON_TYPE_STRING;
        stackVal->pack.obj = melNewString(vm, val, strlen(val));
        return 1;
    }
};

template <>
class Push<std::string>
{
public:
    static TByte push(VM* vm, const std::string& val)
    {
        GCItem* str = melNewString(vm, val.c_str(), val.size());

        melM_stackEnsure(&vm->stack, vm->stack.top + 1);
        Value* stackVal = melM_stackAllocRaw(&vm->stack);
        stackVal->type = MELON_TYPE_STRING;
        stackVal->pack.obj = str;
        return 1;
    }
};

template <typename ...Types>
TByte push(VM* vm, Types ...args)
{
    TByte pushedTotal = 0;

    TByte results[] = {
        Push<Types>::push(vm, std::forward<Types>(args))...
    };

    for (TSize i = 0; i < sizeof(results); i++)
    {
        pushedTotal += results[i];
    }

    return pushedTotal;
}

template <>
class Push<void>
{
public:
    static TByte push(VM* vm)
    {
        return 0;
    }
};

} // namespace ffi
} // namespace melon
} // namespace kola