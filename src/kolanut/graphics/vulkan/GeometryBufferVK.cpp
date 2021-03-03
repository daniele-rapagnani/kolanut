#include "kolanut/graphics/vulkan/GeometryBufferVK.h"
#include "kolanut/graphics/vulkan/utils/Device.h"

#include <vulkan/vulkan.h>

#include <cassert>

namespace kola {
namespace graphics {

bool GeometryBufferVK::createBuffer()
{
    this->buffer = vulkan::make_init<vulkan::Buffer>(
        getDevice(),
        getFrameBufferSize() * getConfig().maxFrames,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    );

    if (!this->buffer)
    {
        return false;
    }

    return true;
}

bool GeometryBufferVK::bind(Handle h, std::shared_ptr<vulkan::CommandBuffer> commandBuf)
{
    uint64_t base = (h >> 32) & 0xFFFFFFFF;
    VkBuffer b = this->buffer->getVkHandle();

    vkCmdBindVertexBuffers(commandBuf->getVkHandle(), 0, 1, &b, &base);

    return true;
}

bool GeometryBufferVK::copy(const Vertex* vertices, size_t base, size_t size, size_t* realSize)
{
    assert(realSize);
    
    if (base + size > this->buffer->getAllocation()->size)
    {
        return false;
    }

    if (!this->buffer->copy(vertices, size, base))
    {
        return false;
    }

    VkDeviceSize alignment = this->buffer->getAllocation()->pool->alignment;
    VkDeviceSize reminder = size % alignment;

    if (reminder > 0)
    {
        size = size + alignment - reminder;
    }

    *realSize = size;

    return true;
}

} // namespace graphics
} // namespace kola