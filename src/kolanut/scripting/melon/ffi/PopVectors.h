#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/PopPrimitives.h"

#include <cmath>

extern "C" {
#include <melon/core/vm.h>
#include <melon/core/object.h>
}

namespace kola {
namespace melon {
namespace ffi {

template <>
inline bool convert(VM* vm, Vec2f& res, Value* val)
{
    if (val->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    return (
        getInstanceField(vm, val->pack.obj, "x", res.x)
        && getInstanceField(vm, val->pack.obj, "y", res.y)
    );
}

template <>
inline bool convert(VM* vm, Vec2i& res, Value* val)
{
    if (val->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    float x;
    float y;

    if (
        !getInstanceField(vm, val->pack.obj, "x", x)
        || !getInstanceField(vm, val->pack.obj, "y", y)
    )
    {
        return false;
    }

    res.x = round(x);
    res.y = round(y);

    return true;
}

template <>
inline bool convert(VM* vm, Sizei& res, Value* val)
{
    Vec2i v;
    
    if (!convert<Vec2i>(vm, v, val))
    {
        return false;
    }

    res.x = v.x;
    res.y = v.y;

    return true;
}

template <>
inline bool convert(VM* vm, Sizef& res, Value* val)
{
    Vec2f v;
    
    if (!convert<Vec2f>(vm, v, val))
    {
        return false;
    }

    res.x = v.x;
    res.y = v.y;

    return true;
}

template <>
inline bool convert(VM* vm, Vec3f& res, Value* val)
{
    if (val->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    return (
        getInstanceField(vm, val->pack.obj, "x", res.x)
        && getInstanceField(vm, val->pack.obj, "y", res.y)
        && getInstanceField(vm, val->pack.obj, "z", res.z)
    );
}

template <>
inline bool convert(VM* vm, glm::vec3& res, Value* val)
{
    if (val->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    return (
        getInstanceField(vm, val->pack.obj, "x", res.x)
        && getInstanceField(vm, val->pack.obj, "y", res.y)
        && getInstanceField(vm, val->pack.obj, "z", res.z)
    );
}

template <>
inline bool convert(VM* vm, glm::vec4& res, Value* val)
{
    if (val->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    return (
        getInstanceField(vm, val->pack.obj, "x", res.x)
        && getInstanceField(vm, val->pack.obj, "y", res.y)
        && getInstanceField(vm, val->pack.obj, "z", res.z)
        && getInstanceField(vm, val->pack.obj, "w", res.w)
    );
}

template <>
inline bool convert(VM* vm, Vec4f& res, Value* val)
{
    if (val->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    return (
        getInstanceField(vm, val->pack.obj, "x", res.x)
        && getInstanceField(vm, val->pack.obj, "y", res.y)
        && getInstanceField(vm, val->pack.obj, "w", res.z)
        && getInstanceField(vm, val->pack.obj, "h", res.w)
    );
}

template <>
inline bool convert(VM* vm, Vec4i& res, Value* val)
{
    if (val->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    float x, y, w, h;

    if (
        !getInstanceField(vm, val->pack.obj, "x", x)
        || !getInstanceField(vm, val->pack.obj, "y", y)
        || !getInstanceField(vm, val->pack.obj, "w", w)
        || !getInstanceField(vm, val->pack.obj, "h", h)
    )
    {
        return false;
    }

    res.x = round(x);
    res.y = round(y);
    res.z = round(w);
    res.w = round(h);

    return true;
}

template <>
inline bool convert(VM* vm, Recti& res, Value* val)
{
    if (val->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    return (
        getInstanceField(vm, val->pack.obj, "origin", res.origin)
        && getInstanceField(vm, val->pack.obj, "size", res.size)
    );
}

template <>
inline bool convert(VM* vm, Rectf& res, Value* val)
{
    if (val->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    return (
        getInstanceField(vm, val->pack.obj, "origin", res.origin)
        && getInstanceField(vm, val->pack.obj, "size", res.size)
    );
}

template <>
inline bool convert(VM* vm, Colori& res, Value* val)
{
    if (val->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    if (
        !getInstanceField(vm, val->pack.obj, "r", res.x)
        || !getInstanceField(vm, val->pack.obj, "g", res.y)
        || !getInstanceField(vm, val->pack.obj, "b", res.z)
        || !getInstanceField(vm, val->pack.obj, "a", res.w)
    )
    {
        return false;
    }

    return true;
}

template <>
inline bool convert(VM* vm, Colorf& res, Value* val)
{
    if (val->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    int32_t r, g, b, a;

    if (
        !getInstanceField(vm, val->pack.obj, "r", r)
        || !getInstanceField(vm, val->pack.obj, "g", g)
        || !getInstanceField(vm, val->pack.obj, "b", b)
        || !getInstanceField(vm, val->pack.obj, "a", a)
    )
    {
        return false;
    }

    return true;
}

} // namespace ffi
} // namespace melon
} // namespace kola