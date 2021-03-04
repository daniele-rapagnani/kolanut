#pragma once

#include "kolanut/graphics/FontSTBTexture.h"
#include "kolanut/graphics/vulkan/TextureVK.h"

namespace kola {
namespace graphics {

class FontVK : public FontSTBTexture<TextureVK>
{ };

} // namespace graphics
} // namespace kola