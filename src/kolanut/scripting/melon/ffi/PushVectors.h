#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/PushPrimitives.h"

extern "C" {
#include <melon/core/vm.h>
#include <melon/core/object.h>
}

namespace kola {
namespace melon {
namespace ffi {

template <>
inline TByte push<Vec2i>(VM* vm, const Vec2i& val)
{
    createInstance(vm, "", "Vector", "create", val.x, val.y);
    return 1;
}

template <>
inline TByte push<Vec2f>(VM* vm, const Vec2f& val)
{
    createInstance(vm, "", "Vector", "create", val.x, val.y);
    return 1;
}

template <>
inline TByte push<Vec4f>(VM* vm, const Vec4f& val)
{
    createInstance(vm, "", "Rect", "create", val.x, val.y, val.z, val.w);
    return 1;
}

template <>
inline TByte push<Vec4i>(VM* vm, const Vec4i& val)
{
    createInstance(vm, "", "Rect", "create", val.x, val.y, val.z, val.w);
    return 1;
}

} // namespace ffi
} // namespace melon
} // namespace kola