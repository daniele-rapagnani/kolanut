#include "kolanut/graphics/vulkan/utils/Fence.h"
#include "kolanut/graphics/vulkan/utils/Device.h"

#include <limits>

namespace kola {
namespace graphics {
namespace vulkan {

bool Fence::init(std::shared_ptr<Device> device, bool signaled /* = false */)
{
    VkFenceCreateInfo fci = {};
    fci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    if (signaled)
    {
        fci.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    this->device = device;

    return vkCreateFence(device->getVkHandle(), &fci, nullptr, &this->handle) == VK_SUCCESS;
}

bool Fence::reset() const
{
    return vkResetFences(getDevice()->getVkHandle(), 1, &this->handle) == VK_SUCCESS;
}

bool Fence::wait(uint64_t timeout /* = 0 */) const
{
    return vkWaitForFences(
        getDevice()->getVkHandle(), 
        1, 
        &this->handle, 
        VK_TRUE,
        timeout == 0 ? std::numeric_limits<uint64_t>::max() : timeout
    ) == VK_SUCCESS;
}

Fence::~Fence()
{
    if (*this)
    {
        vkDestroyFence(this->device->getVkHandle(), this->handle, nullptr);
        this->handle = VK_NULL_HANDLE;
    }
}

} // namespace vulkan
} // namespace graphics
} // namespace kola