#pragma once

#include "kolanut/graphics/vulkan/utils/VkHandle.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"
#include "kolanut/graphics/vulkan/utils/Buffer.h"
#include "kolanut/graphics/vulkan/utils/MemoryManager.h"
#include "kolanut/graphics/vulkan/utils/Queue.h"
#include "kolanut/graphics/vulkan/utils/DescriptorSet.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <cstdint>
#include <vector>

namespace kola {
namespace graphics {
namespace vulkan {

class Texture : public DeviceDependent
{
public:
    bool init(
        std::shared_ptr<Device> device, 
        uint8_t* data,
        size_t width,
        size_t height
    );

    Texture() = default;
    ~Texture();

    bool transferToGPU(std::shared_ptr<Queue> queue) const;

    void use(VkSampler sampler, std::shared_ptr<DescriptorSet> set, uint32_t binding = 0);

    size_t getWidth() const
    { return this->width; }

    size_t getHeight() const
    { return this->height; }

private:
    std::shared_ptr<Buffer> stagingBuffer = {};
    const MemoryManager::Allocation* textureMem = {};
    size_t width = {};
    size_t height = {};

    VkImage image = {};
    VkImageView imageView = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola