#include "kolanut/graphics/vulkan/TextureVK.h"
#include "kolanut/graphics/vulkan/RendererVK.h"
#include "kolanut/graphics/vulkan/utils/Texture.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"

#include <cstdint>

namespace kola {
namespace graphics {

bool TextureVK::load(unsigned char* data, size_t w, size_t h)
{
    std::shared_ptr<RendererVK> renderer = 
        std::static_pointer_cast<RendererVK>(di::get<graphics::Renderer>())
    ;

    this->texture = vulkan::make_init<vulkan::Texture>(renderer->getDevice(), data, w, h);

    if (!this->texture)
    {
        return false;
    }

    return this->texture->transferToGPU(renderer->getGraphicQueue());
}

} // namespace graphics
} // namespace kola