#pragma once

#include <linalg.h>

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

namespace kola {

template <typename T, int M>
using Vec = linalg::vec<T, M>;

template <typename T>
using Vec2 = Vec<T, 2>;

template <typename T>
using Vec3 = Vec<T, 3>;

template <typename T>
using Vec4 = Vec<T, 4>;

using Vec2i = Vec<long, 2>;
using Vec3i = Vec<long, 3>;
using Vec4i = Vec<long, 4>;

using Vec2f = Vec<float, 2>;
using Vec3f = Vec<float, 3>;
using Vec4f = Vec<float, 4>;

using Sizei = Vec2i;
using Sizef = Vec2f;

using Recti = Vec4i;
using Rectf = Vec4f;

struct Colori : public Vec4i {
    Colori(long r = 255, long g = 255, long b = 255, long a = 255) : Vec4i(r, g, b, a) {}
};

struct Colorf : public Vec4f {
    Colorf(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f) : Vec4f(r, g, b, a) {}
};

using Mat3f = glm::mat3x3;
using Mat4f = glm::mat4x4;

using Transform2D = Mat3f;
using Transform3D = Mat4f;

} // namespace kola