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
bool pop(VM* vm, T& res) = delete;

template <>
inline bool pop(VM* vm, bool& res)
{
    if (melM_stackIsEmpty(&vm->stack))
    {
        return false;
    }

    if (melM_stackTop(&vm->stack)->type != MELON_TYPE_BOOL)
    {
        return false;
    }

    res = melM_stackTop(&vm->stack)->pack.value.boolean != 0;
    return true;
}

} // namespace ffi
} // namespace melon
} // namespace kola