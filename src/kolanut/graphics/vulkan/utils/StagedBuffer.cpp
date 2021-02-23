#include "kolanut/graphics/vulkan/utils/StagedBuffer.h"
#include "kolanut/graphics/vulkan/utils/Device.h"

namespace kola {
namespace graphics {
namespace vulkan {

bool StagedBuffer::init(
    std::shared_ptr<Device> device, 
    VkDeviceSize size, 
    VkBufferUsageFlags usage
)
{
    this->staging = make_init<Buffer>(
        device, 
        size, 
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    this->buffer = make_init<Buffer>(
        device, 
        size, 
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    return this->staging && this->buffer;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola