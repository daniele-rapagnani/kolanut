#pragma once

#include "kolanut/graphics/TextureSTB.h"

#include <string>
#include <memory>

namespace kola {
namespace graphics {

class RendererVK;

namespace vulkan {
class Texture;
}

class TextureVK : public TextureSTB
{
public:
    TextureVK() = default;

public:
    bool load(unsigned char* data, size_t w, size_t h);

    std::shared_ptr<vulkan::Texture> getTexture() const
    { return this->texture; }

private:
    std::shared_ptr<vulkan::Texture> texture = nullptr;
};

} // namespace graphics
} // namespace kola