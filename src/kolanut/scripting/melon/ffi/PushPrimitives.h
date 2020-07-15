#pragma once

extern "C" {
#include <melon/core/vm.h>
#include <melon/core/tstring.h>
#include <melon/modules/modules.h>
}

namespace kola {
namespace melon {
namespace ffi {

template <typename T>
TByte push(VM* vm, const T& val) = delete;

template <>
inline TByte push<bool>(VM* vm, const bool& val)
{
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* stackVal = melM_stackAllocRaw(&vm->stack);
    stackVal->type = MELON_TYPE_BOOL;
    stackVal->pack.value.boolean = val ? 1 : 0;

    return 1;
}

template <>
inline TByte push<unsigned long>(VM* vm, const unsigned long& val)
{
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* stackVal = melM_stackAllocRaw(&vm->stack);
    stackVal->type = MELON_TYPE_INTEGER;
    stackVal->pack.value.integer = val;

    return 1;
}

template <>
inline TByte push<long long>(VM* vm, const long long& val)
{
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* stackVal = melM_stackAllocRaw(&vm->stack);
    stackVal->type = MELON_TYPE_INTEGER;
    stackVal->pack.value.integer = val;

    return 1;
}

template <>
inline TByte push<long>(VM* vm, const long& val)
{
    return push<long long>(vm, val);
}

template <>
inline TByte push<int>(VM* vm, const int& val)
{
    return push<long long>(vm, val);
}

template <>
inline TByte push<unsigned int>(VM* vm, const unsigned int& val)
{
    return push<unsigned long>(vm, val);
}

template <>
inline TByte push<double>(VM* vm, const double& val)
{
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* stackVal = melM_stackAllocRaw(&vm->stack);
    stackVal->type = MELON_TYPE_NUMBER;
    stackVal->pack.value.number = val;

    return 1;
}

template <>
inline TByte push<float>(VM* vm, const float& val)
{
    return push<double>(vm, static_cast<double>(val));
}

template <>
inline TByte push<const char*>(VM* vm, const char* const& val)
{
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* stackVal = melM_stackAllocRaw(&vm->stack);
    stackVal->type = MELON_TYPE_STRING;
    stackVal->pack.obj = melNewString(vm, val, strlen(val));
    return 1;
}

template <>
inline TByte push<std::string>(VM* vm, const std::string& val)
{
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* stackVal = melM_stackAllocRaw(&vm->stack);
    stackVal->type = MELON_TYPE_STRING;
    stackVal->pack.obj = melNewString(vm, val.c_str(), val.size());
    return 1;
}

template <typename ...Types>
TByte push(VM* vm, Types ...args)
{
    TByte pushedTotal = 0;

    TByte results[] = {
        push(vm, std::forward<Types>(args))...
    };

    for (TSize i = 0; i < sizeof(results); i++)
    {
        pushedTotal += results[i];
    }

    return pushedTotal;
}

template <>
inline TByte push(VM* vm)
{
    return 0;
}

} // namespace ffi
} // namespace melon
} // namespace kola