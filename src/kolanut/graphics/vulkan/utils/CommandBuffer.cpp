#include "kolanut/graphics/vulkan/utils/CommandBuffer.h"
#include "kolanut/graphics/vulkan/utils/Device.h"

#include <unordered_map>
#include <cassert>

namespace kola {
namespace graphics {
namespace vulkan {

CommandBuffer::~CommandBuffer()
{
    if (*this)
    {
        if (this->begun)
        {
            end();
        }

        vkFreeCommandBuffers(getDevice()->getVkHandle(), this->pool, 1, &this->handle);
        this->handle = VK_NULL_HANDLE;
    }
}

bool CommandBuffer::init(
    std::shared_ptr<Device> device, 
    VkCommandPool pool,
    VkCommandBufferLevel level
)
{
    this->device = device;
    this->pool = pool;
    this->level = level;

    VkCommandBufferAllocateInfo cbai = {};
    cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbai.commandBufferCount = 1;
    cbai.commandPool = pool;
    cbai.level = level;

    if (vkAllocateCommandBuffers(this->device->getVkHandle(), &cbai, &this->handle) != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

bool CommandBuffer::begin(VkCommandBufferUsageFlags usage /* = 0 */)
{
    VkCommandBufferBeginInfo cbbi = {};
    cbbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cbbi.flags = usage;

    if (vkBeginCommandBuffer(getVkHandle(), &cbbi) != VK_SUCCESS)
    {
        return false;
    }

    this->begun = true;

    return true;
}

void CommandBuffer::end()
{
    vkEndCommandBuffer(getVkHandle());
    this->begun = false;
}

bool CommandBuffer::run(
    Runner runner,
    VkCommandBufferUsageFlags usage /* = 0 */
)
{
    if (!begin(usage))
    {
        return false;
    }

    bool success = runner(getVkHandle());

    end();

    return success;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola