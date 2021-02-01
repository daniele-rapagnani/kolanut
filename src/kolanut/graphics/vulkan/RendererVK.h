#pragma once

#include "kolanut/graphics/Renderer.h"

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

    VkInstance instance = {};
    VkSurfaceKHR surface = {};
    VkDebugUtilsMessengerEXT messenger = {};
    VkPhysicalDevice physicalDevice = {};
    VkDevice logicalDevice = {};
    VkQueue presQueue = {};
    VkQueue graphQueue = {};
    VkSwapchainKHR swapchain = {};

    std::vector<VkImage> swapchainImages = {};
    std::vector<VkImageView> swapchainImageViews = {};

    VkShaderModule vertexShader = {};
    VkShaderModule fragmentShader = {};

    VkRenderPass renderPass = {};
    VkPipelineLayout pipelineLayout = {};
    VkPipeline pipeline = {};
    VkCommandPool graphicsCmdPool = {};
    VkCommandPool presentCmdPool = {};

    uint8_t framesInFlight = 2;
    uint8_t currentInFlightFrame = 0;

    std::vector<VkSemaphore> swapchainImageReadySems = {};
    std::vector<VkSemaphore> renderCompleteSems = {};
    std::vector<VkFence> frameCompleteFens = {};

    std::vector<VkCommandBuffer> graphicsCommandBuffers = {};
    std::vector<VkCommandBuffer> presentCommandBuffers = {};

    std::vector<VkFramebuffer> framebuffers = {};
};

} // namespace graphics
} // namespace kola