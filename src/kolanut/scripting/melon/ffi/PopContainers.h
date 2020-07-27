#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/PopPrimitives.h"

extern "C" {
#include <melon/core/vm.h>
#include <melon/core/array.h>
}

#include <vector>

namespace kola {
namespace melon {
namespace ffi {

// @TODO: Refactor to use a class based template in order
//        to have partial specialization.

// template <typename T>
// inline bool convert(VM* vm, std::vector<T>& res, Value* val)
// {
//     if (val->type != MELON_TYPE_ARRAY)
//     {
//         return false;
//     }

//     res.clear();

//     Array* arr = melM_arrayFromObj(val->pack.obj);

//     for (TSize i = 0; i < arr->count; ++i)
//     {
//         Value* item = melGetIndexArray(vm, val->pack.obj, i);
//         T& tval = res.emplace_back();
        
//         if (!convert(vm, tval, item))
//         {
//             res.clear();
//             return false;
//         }
//     }

//     return true;
// }

} // namespace ffi
} // namespace melon
} // namespace kola