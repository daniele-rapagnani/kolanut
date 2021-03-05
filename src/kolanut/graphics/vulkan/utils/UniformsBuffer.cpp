#include "kolanut/graphics/vulkan/utils/UniformsBuffer.h"
#include "kolanut/graphics/vulkan/utils/Device.h"
#include "kolanut/core/Types.h"

namespace kola {
namespace graphics {
namespace vulkan {

bool UniformsBuffer::init(std::shared_ptr<Device> device, const Config& config /* = {} */)
{
    this->device = device;
    this->config = config;
    this->frameBufferSize = sizeof(Vec4f) * getConfig().maxVec4PerSet * getConfig().maxSets;

    this->buffer = make_init<Buffer>(
        getDevice(),
        this->frameBufferSize * getConfig().maxFrames,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
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

UniformsBuffer::Handle UniformsBuffer::addUniform(const void* data, size_t bytes, uint8_t frame)
{
    frame = frame % getConfig().maxFrames;

    VkDeviceSize base = this->bufferBase[static_cast<size_t>(frame)];

    if (base + bytes > this->buffer->getAllocation()->size)
    {
        return NULL_HANDLE;
    }

    if (!this->buffer->copy(data, bytes, base))
    {
        return NULL_HANDLE;
    }

    VkDeviceSize alignment = this->buffer->getAllocation()->pool->alignment;
    VkDeviceSize reminder = bytes % alignment;

    if (reminder > 0)
    {
        bytes = bytes + alignment - reminder;
    }

    this->bufferBase[frame] += bytes;

    return (base << 32) | ((bytes & 0xFFFFFFF) << 4) | ((frame + 1) & 0xF);
}

bool UniformsBuffer::bind(
    Handle h,
    std::shared_ptr<DescriptorSet> descriptorSet,
    uint32_t binding /* = 0 */
)
{
    uint64_t base = (h >> 32) & 0xFFFFFFFF;
    return this->buffer->bind(descriptorSet, binding, base);
}

} // namespace vulkan
} // namespace graphics
} // namespace kola