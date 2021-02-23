
#pragma once

#include "kolanut/graphics/vulkan/utils/Instance.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"
#include "kolanut/graphics/vulkan/utils/QueueFamily.h"
#include "kolanut/graphics/vulkan/utils/PhysicalDevice.h"
#include "kolanut/graphics/vulkan/utils/VkHandle.h"
#include "kolanut/graphics/vulkan/utils/Queue.h"
#include "kolanut/graphics/vulkan/utils/Swapchain.h"
#include "kolanut/graphics/vulkan/utils/RenderPass.h"
#include "kolanut/graphics/vulkan/utils/MemoryManager.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <memory>

namespace kola {
namespace graphics {
namespace vulkan {

class Device : public VkHandle<VkDevice>, public std::enable_shared_from_this<Device>
{
private:
    struct QueueGroup
    {
        uint32_t count = 0;
        std::vector<float> priorities;
        std::vector<Queue::Config> configs;
    };

public:
    struct Config
    {
        std::vector<Queue::Config> queues;
        std::vector<std::string> extensions;
        Swapchain::Config swapchain;
        uint8_t framesInFlight = 2;
    };

public:
    Device() = default;
    ~Device();

    bool init(
        std::shared_ptr<PhysicalDevice> physicalDevice, 
        VkSurfaceKHR surface, 
        const Config& config
    );

    std::shared_ptr<Queue> getQueue(QueueFamily family, uint32_t index = 0) const;

    bool addRenderPass(const RenderPass::Config& config);

    const std::vector<std::shared_ptr<RenderPass>>& getRenderPasses()
    { return this->renderPasses; }

    std::shared_ptr<PhysicalDevice> getPhysicalDevice() const
    { return this->physicalDevice; }

    std::shared_ptr<Swapchain> getSwapchain() const
    { return this->swapchain; }

    std::shared_ptr<MemoryManager> getMemoryManager() const
    { return this->memMgr; }

    const Config& getConfig() const
    { return this->config; }

private:
    Config config = {};
    std::unordered_map<uint32_t, QueueGroup> queueGroups = {};
    std::unordered_map<uint32_t, std::vector<std::shared_ptr<Queue>>> queues = {};

    std::shared_ptr<PhysicalDevice> physicalDevice = {};
    std::shared_ptr<MemoryManager> memMgr = {};
    std::vector<std::shared_ptr<RenderPass>> renderPasses = {};
    std::shared_ptr<Swapchain> swapchain = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola