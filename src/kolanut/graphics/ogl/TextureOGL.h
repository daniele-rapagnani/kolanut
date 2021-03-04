#pragma once

#include "kolanut/graphics/TextureSTB.h"

#include <glad/glad.h>

#include <string>

namespace kola {
namespace graphics {

class RendererOGL;

class TextureOGL : public TextureSTB
{
public:
    TextureOGL() = default;
    ~TextureOGL();

public:
    bool load(unsigned char* data, size_t w, size_t h);
    void bind() const;

private:
    GLuint texture = 0;
};

} // namespace graphics
} // namespace kola