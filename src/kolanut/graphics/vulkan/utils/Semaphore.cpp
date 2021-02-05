#include "kolanut/graphics/vulkan/utils/Semaphore.h"
#include "kolanut/graphics/vulkan/utils/Device.h"

namespace kola {
namespace graphics {
namespace vulkan {

bool Semaphore::init(std::shared_ptr<Device> device)
{
    VkSemaphoreCreateInfo sci = {};
    sci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    this->device = device;

    return vkCreateSemaphore(device->getVkHandle(), &sci, nullptr, &this->handle) == VK_SUCCESS;
}

Semaphore::~Semaphore()
{
    if (*this)
    {
        vkDestroySemaphore(this->device->getVkHandle(), this->handle, nullptr);
        this->handle = VK_NULL_HANDLE;
    }
}

} // namespace vulkan
} // namespace graphics
} // namespace kola