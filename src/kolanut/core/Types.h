#pragma once

#include <linalg.h>

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

} // namespace kola