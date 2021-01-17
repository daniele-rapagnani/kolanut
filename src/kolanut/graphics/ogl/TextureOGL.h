#pragma once

#include "kolanut/graphics/Texture.h"

#include <glad/glad.h>

#include <string>

namespace kola {
namespace graphics {

class RendererOGL;

class TextureOGL : public Texture
{
public:
    friend class RendererOGL;

    TextureOGL() = default;
    ~TextureOGL();

public:
    bool load(const std::string& file) override;
    bool load(unsigned char* data, size_t w, size_t h);
    Sizei getSize() override;

    void bind() const;

private:
    GLuint texture = 0;
    Sizei size = {};
    unsigned char* data = nullptr;
    bool ownsData = false;
};

} // namespace graphics
} // namespace kola