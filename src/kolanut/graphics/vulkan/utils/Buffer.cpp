#include "kolanut/graphics/vulkan/utils/Buffer.h"
#include "kolanut/graphics/vulkan/utils/Device.h"
#include "kolanut/graphics/vulkan/utils/Fence.h"
#include "kolanut/graphics/vulkan/utils/CommandBuffer.h"
#include "kolanut/graphics/vulkan/utils/MappedMemory.h"

#include <cstring>

namespace kola {
namespace graphics {
namespace vulkan {

bool Buffer::init(
    std::shared_ptr<Device> device, 
    VkDeviceSize size, 
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags memoryProps 
        /* = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT */
)
{   
    VkBufferCreateInfo bci = {};
    bci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bci.size = size;
    bci.usage = usage;

    if (vkCreateBuffer(device->getVkHandle(), &bci, nullptr, &this->handle) != VK_SUCCESS)
    {
        return false;
    }

    this->device = device;

    vkGetBufferMemoryRequirements(this->device->getVkHandle(), getVkHandle(), &this->requirements);

    auto a = this->device->getMemoryManager()->allocate(this->requirements, memoryProps);

    if (!a)
    {
        return false;
    }

    this->allocation = a;

    if (vkBindBufferMemory(this->device->getVkHandle(), getVkHandle(), a->ptr, a->offset) != VK_SUCCESS)
    {
        return false;
    }

    return true;
}

bool Buffer::copy(const void* source, VkDeviceSize size, size_t offset /* = 0 */)
{
    if (!getAllocation())
    {
        return false;
    }

    if (getAllocation()->size < offset + size)
    {
        return false;
    }

    assert(reinterpret_cast<uintptr_t>(offset) % this->requirements.alignment == 0);

    vulkan::MappedMemory<float> mem = { shared_from_this(), offset };

    //TODO: change this for write-combined memory to account for
    //      cache line size
    memcpy(*mem, source, size);

    return true;
}

bool Buffer::transferNow(std::shared_ptr<Buffer> dst, std::shared_ptr<Queue> queue)
{
    return queue->submitOneShot(
        [dst, this] (VkCommandBuffer b) {
            VkBufferCopy bc = {};
            bc.size = std::min(dst->getAllocation()->size, getAllocation()->size);

            vkCmdCopyBuffer(b, getVkHandle(), dst->getVkHandle(), 1, &bc);
            return true;
        }
    );
}

bool Buffer::transfer(std::shared_ptr<Buffer> dst, std::shared_ptr<CommandBuffer> cb)
{
    VkBufferCopy bc = {};
    bc.size = std::min(dst->getAllocation()->size, getAllocation()->size);

    vkCmdCopyBuffer(cb->getVkHandle(), getVkHandle(), dst->getVkHandle(), 1, &bc);
    return true;
}

bool Buffer::bind(
    std::shared_ptr<DescriptorSet> descriptorSet, 
    uint32_t binding /* = 0 */,
    VkDeviceSize offset /* = 0 */
)
{
    VkDescriptorBufferInfo dbi = {};
    dbi.buffer = getVkHandle();
    dbi.offset = offset;
    dbi.range = VK_WHOLE_SIZE;

    VkWriteDescriptorSet wds = {};
    wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    wds.dstBinding = binding;
    wds.dstSet = descriptorSet->getVkHandle();
    wds.descriptorCount = 1;
    wds.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    wds.pBufferInfo = &dbi;

    vkUpdateDescriptorSets(this->device->getVkHandle(),1, &wds, 0, nullptr);

    return true;
}

Buffer::~Buffer()
{
    if (*this)
    {
        vkDestroyBuffer(this->device->getVkHandle(), this->handle, nullptr);
        this->handle = VK_NULL_HANDLE;

        if (this->allocation)
        {
            this->allocation->free();
            this->allocation = nullptr;
        }
    }
}

} // namespace vulkan
} // namespace graphics
} // namespace kola