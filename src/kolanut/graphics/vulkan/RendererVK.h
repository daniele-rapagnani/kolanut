#pragma once

#define GLFW_INCLUDE_VULKAN

#include "kolanut/graphics/glfw/RendererGLFW.h"
#include "kolanut/graphics/vulkan/ShaderVK.h"
#include "kolanut/graphics/vulkan/ProgramVK.h"
#include "kolanut/graphics/vulkan/GeometryBufferVK.h"

#include "kolanut/graphics/vulkan/utils/Instance.h"
#include "kolanut/graphics/vulkan/utils/PhysicalDevice.h"
#include "kolanut/graphics/vulkan/utils/Device.h"
#include "kolanut/graphics/vulkan/utils/Queue.h"
#include "kolanut/graphics/vulkan/utils/RenderPass.h"
#include "kolanut/graphics/vulkan/utils/Pipeline2D.h"
#include "kolanut/graphics/vulkan/utils/ShaderModule.h"
#include "kolanut/graphics/vulkan/utils/Buffer.h"
#include "kolanut/graphics/vulkan/utils/StagedBuffer.h"
#include "kolanut/graphics/vulkan/utils/Texture.h"
#include "kolanut/graphics/vulkan/utils/DescriptorPool.h"
#include "kolanut/graphics/vulkan/utils/DescriptorSet.h"
#include "kolanut/graphics/vulkan/utils/MemoryManager.h"
#include "kolanut/graphics/vulkan/utils/CommandBuffer.h"
#include "kolanut/graphics/vulkan/utils/UniformsBuffer.h"
#include "kolanut/graphics/vulkan/utils/QueryPool.h"
#include "kolanut/graphics/vulkan/utils/Semaphore.h"
#include "kolanut/graphics/vulkan/utils/Fence.h"

#include <TracyVulkan.hpp>

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace kola {
namespace graphics {

class RendererVK : public RendererGLFW
{ 
public:
    static constexpr const char* SHADER_FRAG_EXT = ".frag";
    static constexpr const char* SHADER_VERT_EXT = ".vert";

    static constexpr const size_t MAX_DESC_SETS = 16384;
    static constexpr const size_t MAX_DESC_UNIFORM_BUFFERS = 8192;
    static constexpr const size_t MAX_DESC_IMAGE_SAMPLERS = 12384;

public:
    bool doInit(const Config& config) override;
    std::shared_ptr<Program> createProgram(DrawMode mode) override;
    
    void drawSurface(const DrawSurface& req) override;

    void doClear() override;
    void doFlip() override;

    Vec2i getPixelResolution() const override;

    std::shared_ptr<vulkan::Device> getDevice() const
    { return this->device; }

    std::shared_ptr<vulkan::Queue> getGraphicQueue() const
    { return this->graphQueue; }

protected:
    std::shared_ptr<Texture> createTexture() override;
    std::shared_ptr<Font> createFont() override;
    std::shared_ptr<Shader> createShader() override;
    std::shared_ptr<GeometryBuffer> createGeometryBuffer() override;

    std::string getShadersExt() const override
    { return ".vk"; }

private:
    std::shared_ptr<vulkan::Instance> instance = {};
    std::shared_ptr<vulkan::PhysicalDevice> physicalDevice = {};
    std::shared_ptr<vulkan::Device> device = {};
    std::shared_ptr<vulkan::Queue> presQueue = {};
    std::shared_ptr<vulkan::Queue> graphQueue = {};
    
    std::shared_ptr<vulkan::Pipeline2D> pipeline = {};
    std::shared_ptr<vulkan::UniformsBuffer> uniformsBuffer = {};
    std::shared_ptr<vulkan::Texture> kitten = {};
    std::shared_ptr<vulkan::DescriptorPool> descriptorPool = {};
    std::shared_ptr<vulkan::QueryPool> queryPool = {};

    std::vector<std::shared_ptr<vulkan::Semaphore>> imageReadySemaphores = {};
    std::vector<std::shared_ptr<vulkan::Semaphore>> renderCompleteSemaphores = {};
    std::vector<std::shared_ptr<vulkan::Fence>> frameCompletedFence = {};
    std::vector<std::shared_ptr<vulkan::CommandBuffer>> graphCommandBuffers = {};
    std::vector<std::shared_ptr<vulkan::DescriptorSet>> descriptorSets = {};

    VkSurfaceKHR surface = {};

    uint32_t nextImageIdx = 0;
    std::vector<uint64_t> gpuElapsedTimes = {};

    VkSampler sampler = {};

    std::vector<TracyVkCtx> tracyContextes = {};
};

} // namespace graphics
} // namespace kola