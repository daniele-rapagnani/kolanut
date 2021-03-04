#include "kolanut/graphics/ogl/TextureOGL.h"
#include "kolanut/graphics/ogl/RendererOGL.h"
#include "kolanut/graphics/ogl/utils/GenericUtils.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"

#include <cstdint>

namespace kola {
namespace graphics {

TextureOGL::~TextureOGL()
{
    knM_oglCall(glDeleteTextures(1, &this->texture));
}

bool TextureOGL::load(unsigned char* data, size_t w, size_t h)
{
    knM_oglCall(glGenTextures(1, &this->texture));

    if (!this->texture)
    {
        return false;
    }

    knM_oglCall(glBindTexture(GL_TEXTURE_2D, this->texture));

    knM_oglCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    knM_oglCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    knM_oglCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    knM_oglCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    knM_oglCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, reinterpret_cast<void*>(data)));

    return true;
}

void TextureOGL::bind() const
{
    knM_oglCall(glBindTexture(GL_TEXTURE_2D, this->texture));
}

} // namespace graphics
} // namespace kola