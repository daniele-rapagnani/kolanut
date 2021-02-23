#include "kolanut/graphics/vulkan/utils/Queue.h"
#include "kolanut/graphics/vulkan/utils/Device.h"
#include "kolanut/core/Logging.h"

namespace kola {
namespace graphics {
namespace vulkan {

Queue::~Queue()
{
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

    return true;
}

bool Queue::createCommandBuffers(std::vector<std::shared_ptr<CommandBuffer>>& buffers, size_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        buffers.push_back(make_init_fatal<CommandBuffer>(
            getDevice(), 
            getCommandPool(), 
            VK_COMMAND_BUFFER_LEVEL_PRIMARY
        ));
    }

    return true;
}

bool Queue::submit(
    std::shared_ptr<CommandBuffer> commandBuffer,
    const Sync& sync /* = {} */
)
{
    VkSemaphore waitFor = sync.waitFor ? sync.waitFor->getVkHandle() : VK_NULL_HANDLE;
    VkSemaphore signal = sync.completeSignal ? sync.completeSignal->getVkHandle() : VK_NULL_HANDLE;
    VkCommandBuffer cb = commandBuffer->getVkHandle();

    VkSubmitInfo si = {};
    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.commandBufferCount = 1;
    si.pCommandBuffers = &cb;
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

bool Queue::submitOneShot(CommandBuffer::Runner r)
{
    auto cb = make_init_fatal<CommandBuffer>(
        getDevice(), 
        this->commandPool, 
        VK_COMMAND_BUFFER_LEVEL_PRIMARY
    );

    if (!cb->run(r, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT))
    {
        return false;
    }

    Queue::Sync sync = {};
    sync.submitFence = make_init_fatal<Fence>(getDevice());

    submit(cb, sync);

    sync.submitFence->wait();

    return true;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola