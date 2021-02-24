#include "kolanut/graphics/vulkan/utils/QueryPool.h"
#include "kolanut/graphics/vulkan/utils/Device.h"

#include <limits>

namespace kola {
namespace graphics {
namespace vulkan {

bool QueryPool::init(std::shared_ptr<Device> device, const Config& config)
{
    this->config = config;
    this->device = device;

    VkQueryPoolCreateInfo qpci = {};
    qpci.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    qpci.queryCount = config.count;
    qpci.queryType = config.type;

    return vkCreateQueryPool(
        this->device->getVkHandle(), 
        &qpci, 
        nullptr, 
        &this->handle
    ) == VK_SUCCESS;
}

QueryPool::~QueryPool()
{
    if (*this)
    {
        vkDestroyQueryPool(this->device->getVkHandle(), this->handle, nullptr);
        this->handle = VK_NULL_HANDLE;
    }
}

} // namespace vulkan
} // namespace graphics
} // namespace kola