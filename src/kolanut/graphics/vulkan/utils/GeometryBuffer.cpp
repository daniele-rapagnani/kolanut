#include "kolanut/graphics/vulkan/utils/GeometryBuffer.h"
#include "kolanut/graphics/vulkan/utils/Device.h"

namespace kola {
namespace graphics {
namespace vulkan {

bool GeometryBuffer::init(std::shared_ptr<Device> device, const Config& config /* = {} */)
{
    this->device = device;
    this->config = config;
    this->frameBufferSize = sizeof(Vertex) * getConfig().maxVerts;

    this->buffer = make_init<Buffer>(
        getDevice(),
        this->frameBufferSize * getConfig().maxFrames,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    );

    if (!this->buffer)
    {
        return false;
    }

    this->bufferBase.resize(getConfig().maxFrames);
    
    for (uint8_t i = 0; i < getConfig().maxFrames; i++)
    {
        reset(i);
    }

    return true;
}

GeometryBuffer::Handle GeometryBuffer::addVertices(const Vertex* vertices, size_t count, uint8_t frame)
{
    frame = frame % getConfig().maxFrames;

    size_t size = count * sizeof(Vertex);
    VkDeviceSize base = this->bufferBase[static_cast<size_t>(frame)];

    if (base + size > this->buffer->getAllocation()->size)
    {
        return NULL_HANDLE;
    }

    if (!this->buffer->copy(vertices, size, base))
    {
        return NULL_HANDLE;
    }

    VkDeviceSize alignment = this->buffer->getAllocation()->pool->alignment;
    VkDeviceSize reminder = size % alignment;

    if (reminder > 0)
    {
        size = size + alignment - reminder;
    }

    this->bufferBase[frame] += size;

    return (base << 32) | ((size & 0xFFFFFFF) << 4) | ((frame + 1) & 0xF);
}

bool GeometryBuffer::bind(Handle h, std::shared_ptr<CommandBuffer> commandBuf)
{
    uint64_t base = (h >> 32) & 0xFFFFFFFF;
    VkBuffer b = this->buffer->getVkHandle();

    vkCmdBindVertexBuffers(commandBuf->getVkHandle(), 0, 1, &b, &base);

    return true;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola