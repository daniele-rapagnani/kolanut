#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/graphics/vulkan/utils/ShaderModule.h"
#include "kolanut/graphics/vulkan/utils/Pipeline.h"
#include "kolanut/graphics/vulkan/utils/RenderPass.h"

#include <cstdint>

namespace kola {
namespace graphics {
namespace vulkan {

class Device;

class Pipeline2D : public Pipeline
{
public:
    struct Config
    { 
        std::shared_ptr<ShaderModule> vertexShader = {};
        std::shared_ptr<ShaderModule> fragmentShader = {};
        std::shared_ptr<RenderPass> renderPass = {};
        uint32_t viewportWidth = {};
        uint32_t viewportHeight = {};
    };

public:
    Pipeline2D() = default;

    bool init(
       std::shared_ptr<Device> device,
       const Config& config
    );

    const Config& getConfig() const
    { return this->config; }

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout layout = VK_NULL_HANDLE;

private:
    Config config = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola