#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/PushPrimitives.h"

extern "C" {
#include <melon/core/vm.h>
#include <melon/core/array.h>
}

#include <vector>

namespace kola {
namespace melon {
namespace ffi {

template <typename T>
class Push<std::vector<T>>
{
public:
    static TByte push(VM* vm, const std::vector<T>& val)
    {
        GCItem* newArr = melNewArray(vm);
        melM_vstackPushGCItem(&vm->stack, newArr);
        
        for (auto it = val.cbegin(); it != val.cend(); ++it)
        {
            Push<T>::push(vm, *it);
            melPushArray(vm, newArr, melM_stackTop(&vm->stack));
            melM_stackPop(&vm->stack);
        }

        return 1;
    }
};

} // namespace ffi
} // namespace melon
} // namespace kola