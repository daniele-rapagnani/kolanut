#pragma once

#include "kolanut/graphics/FontSTBTexture.h"
#include "kolanut/graphics/ogl/TextureOGL.h"

namespace kola {
namespace graphics {

class FontOGL : public FontSTBTexture<TextureOGL>
{ };

} // namespace graphics
} // namespace kola