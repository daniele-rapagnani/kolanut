#pragma once

#include "kolanut/graphics/vulkan/utils/VkHandle.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"

#include <vector>
#include <cstdint>

namespace kola {
namespace graphics {
namespace vulkan {

class Device;

class RenderPass : public VkHandle<VkRenderPass>, public DeviceDependent
{
public:
    struct Subpass
    {
        VkSubpassDescription description = {};
        std::vector<VkAttachmentReference> attachmentRefs;
    };

    struct Config
    {
        std::vector<VkAttachmentDescription> attachments;
        std::vector<Subpass> subpasses;
    };

public:
    RenderPass() = default;
    ~RenderPass();

    bool init(
        std::shared_ptr<Device> device, 
        const Config& config
    );

    const Config& getConfig() const
    { return this->config; }

private:
    Config config = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola