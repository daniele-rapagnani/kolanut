#pragma once

#include "kolanut/graphics/Renderer.h"
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
#include "kolanut/graphics/vulkan/utils/GeometryBuffer.h"
#include "kolanut/graphics/vulkan/utils/UniformsBuffer.h"
#include "kolanut/graphics/vulkan/utils/QueryPool.h"
#include "kolanut/graphics/vulkan/utils/Semaphore.h"
#include "kolanut/graphics/vulkan/utils/Fence.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace kola {
namespace graphics {

class RendererVK : public Renderer
{ 
public:
    static constexpr const char* SHADER_FRAG_EXT = ".frag";
    static constexpr const char* SHADER_VERT_EXT = ".vert";

    static constexpr const size_t MAX_DESC_SETS = 16384;
    static constexpr const size_t MAX_DESC_UNIFORM_BUFFERS = 8192;
    static constexpr const size_t MAX_DESC_IMAGE_SAMPLERS = 12384;

public:
    ~RendererVK();

public:
    bool doInit(const Config& config) override;
    std::shared_ptr<Texture> loadTexture(const std::string& file) override;
    std::shared_ptr<Font> loadFont(const std::string& file, size_t sizes) override;
    
    void draw(
        std::shared_ptr<Texture> t, 
        const Vec2f& position, 
        float angle, 
        const Vec2f& scale,
        const Vec2f& origin
    ) override;

    void draw(
        std::shared_ptr<Texture> t, 
        const Vec2f& position, 
        float angle, 
        const Vec2f& scale,
        const Vec2f& origin, 
        const Vec4i& rect,
        const Colori& color
    ) override;

    void draw(
        const Rectf& rect,
        const Colori& color
    ) override;

    void draw(
        const Vec2f& a,
        const Vec2f& b,
        const Colori& color
    ) override;

    void clear() override;
    void flip() override;

    Vec2i getResolution() override;

    void setCameraPosition(const Vec2f& pos) override;
    void setCameraZoom(float zoom) override;
    Vec2f getCameraPosition() override;
    float getCameraZoom() override;

    GLFWwindow* getWindow() const
    { return this->window; }

    Vec2i getPixelResolution();
    float getPixelsPerPoint();

    std::shared_ptr<vulkan::Device> getDevice() const
    { return this->device; }

    std::shared_ptr<vulkan::Queue> getGraphicQueue() const
    { return this->graphQueue; }

private:
    Vec2f cameraPos = {};
    float cameraZoom = 1.0f;

    GLFWwindow* window = nullptr;

    std::shared_ptr<vulkan::Instance> instance = {};
    std::shared_ptr<vulkan::PhysicalDevice> physicalDevice = {};
    std::shared_ptr<vulkan::Device> device = {};
    std::shared_ptr<vulkan::Queue> presQueue = {};
    std::shared_ptr<vulkan::Queue> graphQueue = {};
    std::shared_ptr<vulkan::ShaderModule> vertexShader = {};
    std::shared_ptr<vulkan::ShaderModule> fragmentShader = {};
    std::shared_ptr<vulkan::Pipeline2D> pipeline = {};
    std::shared_ptr<vulkan::GeometryBuffer> geomBuffer = {};
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

    uint8_t currentInFlightFrame = 0;
    uint32_t nextImageIdx = 0;
    std::vector<uint64_t> gpuElapsedTimes = {};

    VkSampler sampler = {};
    Config config = {};
};

} // namespace graphics
} // namespace kola