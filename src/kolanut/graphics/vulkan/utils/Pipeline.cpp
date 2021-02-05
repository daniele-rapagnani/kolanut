#include "kolanut/graphics/vulkan/utils/Pipeline.h"
#include "kolanut/graphics/vulkan/utils/Device.h"

namespace kola {
namespace graphics {
namespace vulkan {

Pipeline::~Pipeline()
{
    if (*this)
    {
        vkDestroyPipeline(getDevice()->getVkHandle(), getVkHandle(), nullptr);
        this->handle = VK_NULL_HANDLE;
    }
}

} // namespace vulkan
} // namespace graphics
} // namespace kola