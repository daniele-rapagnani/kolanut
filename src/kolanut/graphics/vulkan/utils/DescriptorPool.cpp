#include "kolanut/graphics/vulkan/utils/DescriptorPool.h"
#include "kolanut/graphics/vulkan/utils/DescriptorSet.h"
#include "kolanut/graphics/vulkan/utils/Device.h"

#include <limits>

namespace kola {
namespace graphics {
namespace vulkan {

DescriptorPool::~DescriptorPool()
{
    if (*this)
    {
        vkDestroyDescriptorPool(this->device->getVkHandle(), this->handle, nullptr);
        this->handle = VK_NULL_HANDLE;
    }
}

bool DescriptorPool::init(std::shared_ptr<Device> device, const Config& config)
{
    this->device = device;
    this->config = config;

    VkDescriptorPoolCreateInfo dpci = {};
    dpci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    dpci.maxSets = this->config.maxSets;
    dpci.poolSizeCount = this->config.sizes.size();
    dpci.pPoolSizes = this->config.sizes.data();

    return vkCreateDescriptorPool(
        this->device->getVkHandle(), 
        &dpci, 
        nullptr, 
        &this->handle
    ) == VK_SUCCESS;
}

bool DescriptorPool::reset()
{
    return vkResetDescriptorPool(getDevice()->getVkHandle(), getVkHandle(), 0) == VK_SUCCESS;
}

std::shared_ptr<DescriptorSet> DescriptorPool::allocateSet(
    VkDescriptorSetLayout layout, 
    bool ownedByCaller /* = false */
)
{
    return vulkan::make_init_fatal<DescriptorSet>(
        getDevice(), 
        shared_from_this(), 
        layout,
        ownedByCaller
    );
}

} // namespace vulkan
} // namespace graphics
} // namespace kola