#include "kolanut/graphics/vulkan/utils/RenderPass.h"
#include "kolanut/graphics/vulkan/utils/PhysicalDevice.h"
#include "kolanut/graphics/vulkan/utils/Device.h"

#include <cassert>

namespace kola {
namespace graphics {
namespace vulkan {

RenderPass::~RenderPass()
{
    if (*this)
    {
        vkDestroyRenderPass(this->device->getVkHandle(), getVkHandle(), nullptr);
        this->handle = VK_NULL_HANDLE;
    }
}

bool RenderPass::init(
    std::shared_ptr<Device> device, 
    const Config& config
)
{
    assert(!config.attachments.empty());
    assert(!config.subpasses.empty());

    std::vector<VkSubpassDescription> subpasses;

    for (const Subpass& sd : config.subpasses)
    {
        subpasses.push_back(sd.description);
        subpasses.back().colorAttachmentCount = sd.attachmentRefs.size();
        subpasses.back().pColorAttachments = &sd.attachmentRefs[0];
    }

    VkRenderPassCreateInfo rpci = {};
    rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpci.attachmentCount = config.attachments.size();
    rpci.pAttachments = &config.attachments[0];
    rpci.subpassCount = subpasses.size();
    rpci.pSubpasses = &subpasses[0];

    if (vkCreateRenderPass(device->getVkHandle(), &rpci, nullptr, &this->handle) != VK_SUCCESS)
    {
        knM_logFatal("Can't create render pass");
        return false;
    }

    this->config = config;
    this->device = device;

    return true;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola