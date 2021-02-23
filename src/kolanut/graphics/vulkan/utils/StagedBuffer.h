#pragma once

#include "kolanut/graphics/vulkan/utils/Buffer.h"
#include "kolanut/graphics/vulkan/utils/Queue.h"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <memory>

namespace kola {
namespace graphics {
namespace vulkan {

class StagedBuffer
{
public:
    bool init(
        std::shared_ptr<Device> device, 
        VkDeviceSize size, 
        VkBufferUsageFlags usage
    );

    template <typename T>
    bool init(
        std::shared_ptr<Device> device,
        VkBufferUsageFlags usage
    )
    {
        return init(device, sizeof(T), usage);
    }

    std::shared_ptr<Buffer> getStagingBuffer() const
    { return this->staging; }

    std::shared_ptr<Buffer> getBuffer() const
    { return this->buffer; }

    StagedBuffer() = default;

private:
    std::shared_ptr<Buffer> staging = {};
    std::shared_ptr<Buffer> buffer = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola