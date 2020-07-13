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

template <typename ...Types>
TByte push(VM* vm, Types ...args)
{
    TByte pushedTotal = 0;
    std::function<void(TByte)> dummy = 
        [&pushedTotal] (TByte total) { pushedTotal = total; }
    ;

    dummy(push(&vm, std::forward<Types>(args))...);

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