#include "kolanut/graphics/ogl/TextureOGL.h"
#include "kolanut/graphics/ogl/RendererOGL.h"
#include "kolanut/graphics/ogl/utils/GenericUtils.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

#include <cstdint>

namespace kola {
namespace graphics {

TextureOGL::~TextureOGL()
{
    knM_oglCall(glDeleteTextures(1, &this->texture));
}

bool TextureOGL::load(const std::string& file)
{
    std::vector<char> buffer;
    
    if (!di::get<filesystem::FilesystemEngine>()->getFileContent(file, buffer))
    {
        knM_logError("Can't read '" << file << "'");
        return false;
    }

    int w, h, n;

    unsigned char* data = stbi_load_from_memory(
        reinterpret_cast<const unsigned char*>(&buffer[0]), 
        buffer.size(), 
        &w, 
        &h, 
        &n, 
        STBI_rgb_alpha
    );
    
    if (!data)
    {
        knM_logError("Can't load '" << file << "': " << stbi_failure_reason());
        return false;
    }

    if (!load(data, w, h))
    {
        knM_logError("Can't create OGL texture from '" << file << "'");
        return false;
    }
    
    this->ownsData = true;
    
    return true;
}

bool TextureOGL::load(unsigned char* data, size_t w, size_t h)
{
    this->data = data;
    this->ownsData = false;

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

    this->size = { w, h };

    return true;
}

void TextureOGL::bind() const
{
    knM_oglCall(glBindTexture(GL_TEXTURE_2D, this->texture));
}

Sizei TextureOGL::getSize()
{
    return this->size;
}

} // namespace graphics
} // namespace kola