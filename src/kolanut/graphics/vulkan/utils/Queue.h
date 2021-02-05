#pragma once

#include "kolanut/graphics/vulkan/utils/VkHandle.h"
#include "kolanut/graphics/vulkan/utils/QueueFamily.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"
#include "kolanut/graphics/vulkan/utils/Semaphore.h"
#include "kolanut/graphics/vulkan/utils/Fence.h"

#include <cstdint>
#include <vector>
#include <memory>

namespace kola {
namespace graphics {
namespace vulkan {

class Queue : public VkHandle<VkQueue>, public DeviceDependent
{
public:
    friend class Device;

public:
    struct Config
    {
        QueueFamily family = {};
        float priority = 0.0f;
        size_t commandBuffersCount = 0;
    };

    struct Sync
    {
        // Workaround for: 
        // https://stackoverflow.com/questions/43819314/default-member-initializer-needed-within-definition-of-enclosing-class-outside
        Sync() {}

        std::shared_ptr<Fence> submitFence = {};
        std::shared_ptr<Semaphore> waitFor = {};
        std::shared_ptr<Semaphore> completeSignal = {};
        
        VkPipelineStageFlags waitFlags = 
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        ;
    };

public:
    Queue() = default;
    ~Queue();

    bool submit(
        uint32_t commandBufferIdx,
        const Sync& sync = {}
    );

    QueueFamily getFamily() const
    { return this->config.family; }

    VkCommandPool getCommandPool() const
    { return this->commandPool; }

    const std::vector<VkCommandBuffer>& getCommandBuffers() const
    { return this->commandBuffers; }

    const Config& getConfig() const
    { return this->config; }

private:
    bool init(
        std::shared_ptr<Device> device,
        const Config& config
    );

private:
    Config config = {};
    
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
};

} // namespace vulkan
} // namespace graphics
} // namespace kola