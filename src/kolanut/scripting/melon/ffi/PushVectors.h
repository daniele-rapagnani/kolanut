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
class Push<Vec2i>
{
public:
    static TByte push(VM* vm, const Vec2i& val)
    {
        createInstance(vm, "", "Vector2", "create", val.x, val.y);
        return 1;
    }
};

template <>
class Push<Sizei>
{
public:
    static TByte push(VM* vm, const Sizei& val)
    {
        createInstance(vm, "", "Vector2", "create", val.x, val.y);
        return 1;
    }
};

template <>
class Push<Sizef>
{
public:
    static TByte push(VM* vm, const Sizef& val)
    {
        createInstance(vm, "", "Vector2", "create", val.x, val.y);
        return 1;
    }
};

template <>
class Push<Vec2f>
{
public:
    static TByte push(VM* vm, const Vec2f& val)
    {
        createInstance(vm, "", "Vector2", "create", val.x, val.y);
        return 1;
    }
};

template <>
class Push<Vec3f>
{
public:
    static TByte push(VM* vm, const Vec3f& val)
    {
        createInstance(vm, "", "Vector3", "create", val.x, val.y);
        return 1;
    }
};

template <>
class Push<glm::vec3>
{
public:
    static TByte push(VM* vm, const glm::vec3& val)
    {
        createInstance(vm, "", "Vector3", "create", val.x, val.y);
        return 1;
    }
};

template <>
class Push<Vec4f>
{
public:
    static TByte push(VM* vm, const Vec4f& val)
    {
        createInstance(vm, "", "Vector4", "create", val.x, val.y, val.z, val.w);
        return 1;
    }
};

template <>
class Push<Vec4i>
{
public:
    static TByte push(VM* vm, const Vec4i& val)
    {
        createInstance(vm, "", "Vector4", "create", val.x, val.y, val.z, val.w);
        return 1;
    }
};

template <>
class Push<Recti>
{
public:
    static TByte push(VM* vm, const Recti& val)
    {
        createInstance(vm, "", "Rect", "create", val.origin, val.size);
        return 1;
    }
};

template <>
class Push<Rectf>
{
public:
    static TByte push(VM* vm, const Rectf& val)
    {
        createInstance(vm, "", "Rect", "create", val.origin, val.size);
        return 1;
    }
};

template <>
class Push<Colori>
{
public:
    static TByte push(VM* vm, const Colori& val)
    {
        createInstance(vm, "", "Color", "create", val.x, val.y, val.z, val.w);
        return 1;
    }
};

template <>
class Push<Colorf>
{
public:
    static TByte push(VM* vm, const Colorf& val)
    {
        createInstance(vm, "", "Color", "create", val.x, val.y, val.z, val.w);
        return 1;
    }
};

} // namespace ffi
} // namespace melon
} // namespace kola