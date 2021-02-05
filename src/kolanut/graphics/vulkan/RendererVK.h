#pragma once

#include "kolanut/graphics/Renderer.h"
#include "kolanut/graphics/vulkan/utils/Instance.h"
#include "kolanut/graphics/vulkan/utils/PhysicalDevice.h"
#include "kolanut/graphics/vulkan/utils/Device.h"
#include "kolanut/graphics/vulkan/utils/Queue.h"
#include "kolanut/graphics/vulkan/utils/RenderPass.h"
#include "kolanut/graphics/vulkan/utils/Pipeline2D.h"
#include "kolanut/graphics/vulkan/utils/ShaderModule.h"

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

    VkSurfaceKHR surface = {};

    uint8_t currentInFlightFrame = 0;
};

} // namespace graphics
} // namespace kola