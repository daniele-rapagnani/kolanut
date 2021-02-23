#include "kolanut/graphics/vulkan/utils/DescriptorSet.h"
#include "kolanut/graphics/vulkan/utils/DescriptorPool.h"
#include "kolanut/graphics/vulkan/utils/CommandBuffer.h"
#include "kolanut/graphics/vulkan/utils/Device.h"

#include <limits>
#include <cassert>

namespace kola {
namespace graphics {
namespace vulkan {

DescriptorSet::~DescriptorSet()
{
    if (*this)
    {
        if (this->freeIndividually)
        {
            VkResult r = vkFreeDescriptorSets(
                this->device->getVkHandle(), 
                this->pool->getVkHandle(), 
                1,
                &this->handle
            );

            assert(r == VK_SUCCESS);
        }
        
        this->handle = VK_NULL_HANDLE;
    }
}

bool DescriptorSet::init(
    std::shared_ptr<Device> device, 
    std::shared_ptr<DescriptorPool> pool,
    VkDescriptorSetLayout layout,
    bool freeIndividually /* = false */
)
{
    this->device = device;
    this->pool = pool;
    this->layout = layout;
    this->freeIndividually = freeIndividually;

    VkDescriptorSetAllocateInfo dsai = {};
    dsai.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    dsai.descriptorSetCount = 1;
    dsai.descriptorPool = pool->getVkHandle();
    dsai.pSetLayouts = &layout;

    return vkAllocateDescriptorSets(this->device->getVkHandle(), &dsai, &this->handle) 
        == VK_SUCCESS
    ;
}

void DescriptorSet::bind(
    std::shared_ptr<CommandBuffer> cb, 
    VkPipelineLayout pipelineLayout,
    VkPipelineBindPoint bindPoint
)
{
   bind(cb->getVkHandle(), pipelineLayout, bindPoint);
}

void DescriptorSet::bind(
    VkCommandBuffer cb, 
    VkPipelineLayout pipelineLayout,
    VkPipelineBindPoint bindPoints
)
{
     vkCmdBindDescriptorSets(
        cb, 
        VK_PIPELINE_BIND_POINT_GRAPHICS, 
        pipelineLayout, 
        0, 
        1, 
        & this->handle,
        0, 
        nullptr
    );
}

} // namespace vulkan
} // namespace graphics
} // namespace kola