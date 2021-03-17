#pragma once

#include "kolanut/core/Types.h"

namespace kola {
namespace graphics {

struct Vertex
{
    Vec2f pos;
    Vec2f uv;
    Colorf color;
};

static_assert(sizeof(Vertex) == 4 * 8, "Wrong vertex size");

} // namespace graphics
} // namespace kola