#pragma once

#include "kolanut/graphics/Texture.h"

#include <string>

namespace kola {
namespace graphics {

class RendererVK;

class TextureVK : public Texture
{
public:
    friend class RendererVK;

    TextureVK() = default;
    ~TextureVK();

public:
    bool load(const std::string& file) override;
    bool load(unsigned char* data, size_t w, size_t h);
    Sizei getSize() override;

private:
    Sizei size = {};
    unsigned char* data = nullptr;
    bool ownsData = false;
};

} // namespace graphics
} // namespace kola