#pragma once

#include "kolanut/graphics/vulkan/utils/VkHandle.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <cstdint>
#include <vector>

namespace kola {
namespace graphics {
namespace vulkan {

class DescriptorSet;

class DescriptorPool : 
    public VkHandle<VkDescriptorPool>
    , public DeviceDependent
    , public std::enable_shared_from_this<DescriptorPool>
{
public:
    struct Config
    {
        std::vector<VkDescriptorPoolSize> sizes = {};
        size_t maxSets = 1;
    };

public:
    DescriptorPool() = default;
    ~DescriptorPool();

    bool init(std::shared_ptr<Device> device, const Config& config);

    bool reset();

    std::shared_ptr<DescriptorSet> allocateSet(
        VkDescriptorSetLayout layout,
        bool ownedByCaller = false
    );

    const Config& getConfig() const
    { return this->config; }

private:
    Config config = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola