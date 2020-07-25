#pragma once

extern "C" {
#include <melon/core/vm.h>
#include <melon/core/tstring.h>
#include <melon/modules/modules.h>
}

namespace kola {
namespace melon {
namespace ffi {

inline bool checkType(VM* vm, TType type, StackSize offset)
{
    if (vm->stack.top <= offset)
    {
        return false;
    }

    return melM_stackOffset(&vm->stack, offset)->type == type;
}

template <typename T>
bool convert(VM* vm, T& res, Value* val) = delete;

template <typename T>
bool get(VM* vm, T& res, StackSize offset = 0)
{
    if (vm->stack.top <= offset)
    {
        return false;
    }

    return convert(vm, res, melM_stackOffset(&vm->stack, offset));
}

template <typename T>
bool pop(VM* vm, T& res)
{
    if (!get(vm, res))
    {
        return false;
    }

    melM_stackPop(&vm->stack);
    return true;
}

template <>
inline bool convert(VM* vm, bool& res, Value* val)
{
    if (val->type != MELON_TYPE_BOOL)
    {
        return false;
    }

    res = val->pack.value.boolean != 0;
    return true;
}

template <>
inline bool convert(VM* vm, float& res, Value* val)
{
    if (val->type != MELON_TYPE_NUMBER && val->type != MELON_TYPE_INTEGER)
    {
        return false;
    }

    res = val->type == MELON_TYPE_NUMBER ? 
        val->pack.value.number 
        : val->pack.value.integer
    ;
    return true;
}

} // namespace ffi
} // namespace melon
} // namespace kola