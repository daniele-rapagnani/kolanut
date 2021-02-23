#pragma once

#include "kolanut/core/Types.h"

namespace kola {
namespace graphics {
namespace vulkan {

struct Vertex
{
    Vec2f pos;
    Vec2f uv;
    Colorf color;
};

} // namespace vulkan
} // namespace graphics
} // namespace kola