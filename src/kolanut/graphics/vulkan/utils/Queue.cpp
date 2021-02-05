#include "kolanut/graphics/vulkan/utils/Queue.h"
#include "kolanut/graphics/vulkan/utils/Device.h"
#include "kolanut/core/Logging.h"

namespace kola {
namespace graphics {
namespace vulkan {

Queue::~Queue()
{
    if (!this->commandBuffers.empty())
    {
        vkFreeCommandBuffers(
            this->device->getVkHandle(), 
            this->commandPool,
            this->commandBuffers.size(),
            &this->commandBuffers[0]
        );

        this->commandBuffers.clear();
    }

    if (this->commandPool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(this->device->getVkHandle(), this->commandPool, nullptr);
        this->commandPool = VK_NULL_HANDLE;
    }
}

bool Queue::init(
    std::shared_ptr<Device> device,
    const Config& config
)
{
    this->device = device;

    VkCommandPoolCreateInfo cpci = {};
    cpci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cpci.queueFamilyIndex = config.family.index;

    if (vkCreateCommandPool(this->device->getVkHandle(), &cpci, nullptr, &this->commandPool) != VK_SUCCESS)
    {
        knM_logFatal("Can't create command pool for family: " << config.family.index);
        return false;
    }

    VkCommandBufferAllocateInfo cbai = {};
    cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbai.commandBufferCount = config.commandBuffersCount;
    cbai.commandPool = this->commandPool;
    cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    this->commandBuffers.resize(cbai.commandBufferCount);

    if (vkAllocateCommandBuffers(this->device->getVkHandle(), &cbai, &this->commandBuffers[0]) != VK_SUCCESS)
    {
        knM_logFatal(
            "Can't create " 
            << cbai.commandBufferCount 
            << " command buffers for family: " 
            << config.family.index
        );

        return false;
    }

    return true;
}

bool Queue::submit(
    uint32_t commandBufferIdx,
    const Sync& sync /* = {} */
)
{
    assert(commandBufferIdx < this->commandBuffers.size());

    VkSemaphore waitFor = sync.waitFor ? sync.waitFor->getVkHandle() : VK_NULL_HANDLE;
    VkSemaphore signal = sync.completeSignal ? sync.completeSignal->getVkHandle() : VK_NULL_HANDLE;

    VkSubmitInfo si = {};
    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.commandBufferCount = 1;
    si.pCommandBuffers = &getCommandBuffers()[commandBufferIdx];
    si.pWaitSemaphores = &waitFor;
    si.waitSemaphoreCount = waitFor != VK_NULL_HANDLE ? 1 : 0;
    si.pWaitDstStageMask = &sync.waitFlags;
    si.signalSemaphoreCount = signal != VK_NULL_HANDLE ? 1 : 0;
    si.pSignalSemaphores = &signal;
    
    return vkQueueSubmit(
        getVkHandle(), 
        1, 
        &si, 
        sync.submitFence ? sync.submitFence->getVkHandle() : VK_NULL_HANDLE
    ) == VK_SUCCESS;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola