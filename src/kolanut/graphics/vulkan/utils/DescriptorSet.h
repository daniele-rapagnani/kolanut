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

class DescriptorPool;
class CommandBuffer;

class DescriptorSet : 
    public VkHandle<VkDescriptorSet>
    , public DeviceDependent
{
public:
    DescriptorSet() = default;
    ~DescriptorSet();

    bool init(
        std::shared_ptr<Device> device, 
        std::shared_ptr<DescriptorPool> pool,
        VkDescriptorSetLayout layout,
        bool freeIndividually = false
    );

    void bind(
        std::shared_ptr<CommandBuffer> cb, 
        VkPipelineLayout pipelineLayout,
        VkPipelineBindPoint bindPoints
    );

    void bind(
        VkCommandBuffer cb, 
        VkPipelineLayout pipelineLayout,
        VkPipelineBindPoint bindPoints
    );

private:
    std::shared_ptr<DescriptorPool> pool = {};
    VkDescriptorSetLayout layout = {};
    bool freeIndividually = false;
};

} // namespace vulkan
} // namespace graphics
} // namespace kola