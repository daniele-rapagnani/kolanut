#pragma once

#include "kolanut/graphics/Texture.h"

#include <string>
#include <memory>

namespace kola {
namespace graphics {

class RendererVK;

namespace vulkan {
class Texture;
}

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

    std::shared_ptr<vulkan::Texture> getTexture() const
    { return this->texture; }

private:
    Sizei size = {};
    std::shared_ptr<vulkan::Texture> texture = nullptr;
};

} // namespace graphics
} // namespace kola