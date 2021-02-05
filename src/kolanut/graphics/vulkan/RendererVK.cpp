#include "kolanut/graphics/vulkan/RendererVK.h"
#include "kolanut/graphics/vulkan/TextureVK.h"
#include "kolanut/graphics/vulkan/FontVK.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

#include <cstring>
#include <sstream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <functional>
#include <limits>

namespace kola {
namespace graphics {

RendererVK::~RendererVK()
{
    if (this->window)
    {
        glfwDestroyWindow(this->window);
        this->window = nullptr;
    }
}

namespace {

void onWindowResize(GLFWwindow* window, int width, int height)
{
    auto r = std::static_pointer_cast<RendererVK>(di::get<Renderer>());
    //r->updateWindowSize();
}

void onVulkanValidationLayerMessage(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const std::string& message
)
{
    switch(messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            knM_logInfo("[Vulkan] " << message);
            break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            knM_logWarning("[Vulkan] " << message);
            break;

        default:
            knM_logError("[Vulkan] " << message);
            break;
    }
}

} // namespace 

bool RendererVK::doInit(const Config& config)
{
    knM_logDebug("Initilizing Vulkan renderer");

    if (!glfwInit())
    {
        knM_logFatal("Can't init GLFW");
        return false;
    }

    GLFWmonitor* monitor = nullptr;

    if (config.resolution.fullScreen)
    {
        int monitorsCount = 0;
        GLFWmonitor** monitors = glfwGetMonitors(&monitorsCount);

        if (!monitors || monitorsCount == 0)
        {
            knM_logFatal("GLFW couldn't enumerate monitors or none is connected.");
            return false;
        }

        monitor = monitors[0];
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    this->window = glfwCreateWindow(
        config.resolution.screenSize.x,
        config.resolution.screenSize.y,
        config.windowTitle.c_str(), 
        monitor, 
        NULL
    );

    if (!this->window)
    {
        glfwTerminate();

        knM_logFatal("Can't create window with GLFW");
        return false;
    }

    glfwSetWindowAspectRatio(
        this->window, 
        config.resolution.screenSize.x, 
        config.resolution.screenSize.y
    );

    uint32_t glfwExtensionsCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

    vulkan::Instance::Config instConf = {};
    instConf.appName = config.windowTitle;
    instConf.layers.push_back("VK_LAYER_KHRONOS_validation");
    instConf.extensions.push_back("VK_EXT_debug_utils");
    instConf.extensions.push_back("VK_KHR_get_physical_device_properties2");
    instConf.messengerCb = onVulkanValidationLayerMessage;

    std::copy(
        glfwExtensions, 
        glfwExtensions + glfwExtensionsCount, 
        std::back_inserter(instConf.extensions)
    );

    this->instance = std::make_shared<vulkan::Instance>();

    if (!this->instance->init(instConf))
    {
        return false;
    }

    if (
        glfwCreateWindowSurface(this->instance->getVkHandle(), this->window, nullptr, &this->surface)
        != VK_SUCCESS
    )
    {
        knM_logFatal("Can't create Vulkan surface");
        return false;
    }

    if (vulkan::PhysicalDevice::getDevices(this->instance).empty())
    {
        knM_logFatal("No GPUs found on this system");
        return false;
    }

    this->physicalDevice = vulkan::PhysicalDevice::getDevices(this->instance)[0];

    if (!this->physicalDevice->hasSwapchainSupport())
    {
        knM_logFatal(this->physicalDevice->getName() << " doesn't support swapchains.");
        return false;
    }

    vulkan::QueueFamily presentationQueueFamilyIdx =
        this->physicalDevice->getPresentationQueueFamily(this->surface)
    ;

    if (!presentationQueueFamilyIdx)
    {
        knM_logFatal("Can't find a suitable presentation queue");
        return false;
    }

    vulkan::QueueFamily graphicsQueueFamilyIdx = 
        this->physicalDevice->getQueueFamily(vulkan::QueueFamilyType::GRAPHICS)
    ;

    if (!graphicsQueueFamilyIdx)
    {
        knM_logFatal("Can't find a suitable graphics queue");
        return false;
    }

    vulkan::Device::Config devConfig = {};
    devConfig.extensions.push_back("VK_KHR_swapchain");
    
    if (this->physicalDevice->isExtensionSupported("VK_KHR_portability_subset"))
    {
        devConfig.extensions.push_back("VK_KHR_portability_subset");
    }

    vulkan::Queue::Config queueConfig = {};

    queueConfig.family = presentationQueueFamilyIdx;
    devConfig.queues.push_back(queueConfig);

    if (presentationQueueFamilyIdx != graphicsQueueFamilyIdx)
    {
        queueConfig.family = graphicsQueueFamilyIdx;
        devConfig.queues.push_back(queueConfig);
    }

    this->device = std::make_shared<vulkan::Device>();

    if (!this->device->init(this->physicalDevice,this->surface, devConfig))
    {
        return false;
    }

    this->presQueue = this->device->getQueue(presentationQueueFamilyIdx);
    this->graphQueue = (presentationQueueFamilyIdx != graphicsQueueFamilyIdx)
        ? this->device->getQueue(graphicsQueueFamilyIdx)
        : this->presQueue
    ;

    this->vertexShader = std::make_shared<vulkan::ShaderModule>();

    if (
        !this->vertexShader->init(
            this->device,
            di::get<filesystem::FilesystemEngine>(),
            "assets/mainv.svert"
        )
    )
    {
        knM_logFatal("Can't create main vertex shader");
        return false;
    }

    this->fragmentShader = std::make_shared<vulkan::ShaderModule>();

    if (
        !this->fragmentShader->init(
            this->device,
            di::get<filesystem::FilesystemEngine>(),
            "assets/mainv.sfrag"
        )
    )
    {
        knM_logFatal("Can't create main fragment shader");
        return false;
    }

    vulkan::RenderPass::Config renderPassConfig;

    VkAttachmentDescription atd = {};
    atd.format = this->device->getSwapchain()->getConfig().format;
    atd.samples = VK_SAMPLE_COUNT_1_BIT;
    atd.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    atd.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    atd.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    atd.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    renderPassConfig.attachments.push_back(atd);

    vulkan::RenderPass::Subpass subpass;

    VkAttachmentReference atr = {};
    atr.attachment = 0;
    atr.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpass.attachmentRefs.push_back(atr);
    renderPassConfig.subpasses.push_back(subpass);

    if (!this->device->addRenderPass(renderPassConfig))
    {
        return false;
    }

    this->device->getSwapchain()->initFramebuffers(this->device->getRenderPasses().back());

    VkSurfaceCapabilitiesKHR sc = this->physicalDevice->getSurfaceCapabilities(this->surface);

    vulkan::Pipeline2D::Config p2dConf = {};
    p2dConf.fragmentShader = this->fragmentShader;
    p2dConf.vertexShader = this->vertexShader;
    p2dConf.renderPass = this->device->getRenderPasses().back();
    p2dConf.viewportWidth = sc.currentExtent.width;
    p2dConf.viewportHeight = sc.currentExtent.height;

    this->pipeline = std::make_shared<vulkan::Pipeline2D>();

    if (!this->pipeline->init(this->device, p2dConf))
    {
        return false;
    }

    for (auto it = this->device->getSwapchain()->getImageViews().cbegin(); it != this->device->getSwapchain()->getImageViews().cend(); ++it)
    {
        for (uint8_t frame = 0; frame < this->device->getConfig().framesInFlight; frame++)
        {
            size_t idx = std::distance(this->device->getSwapchain()->getImageViews().cbegin(), it);
            size_t offset = this->device->getSwapchain()->getResourceOffset(idx, frame);

            VkCommandBuffer gcb = this->graphQueue->getCommandBuffers()[offset];
            VkCommandBuffer pcb = this->presQueue->getCommandBuffers()[offset];
    
            VkCommandBufferBeginInfo cbbi = {};
            cbbi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(gcb, &cbbi) != VK_SUCCESS)
            {
                knM_logFatal("Can't begin graphics command buffer");
                return false;
            }

            VkClearValue cv = {};
            cv.color.float32[0] = 0.0f;
            cv.color.float32[1] = 0.0f;
            cv.color.float32[2] = 0.0f;
            cv.color.float32[3] = 1.0f;

            VkRenderPassBeginInfo rpbi = {};
            rpbi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rpbi.renderPass = this->device->getRenderPasses().back()->getVkHandle();
            rpbi.framebuffer = this->device->getSwapchain()->getFramebuffer(idx, frame);
            rpbi.renderArea.extent = sc.currentExtent;
            rpbi.clearValueCount = 1;
            rpbi.pClearValues = &cv;

            assert(rpbi.framebuffer);
            vkCmdBeginRenderPass(gcb, &rpbi, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(gcb, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline->getVkHandle());
            vkCmdDraw(gcb, 3, 1, 0, 0);

            vkCmdEndRenderPass(gcb);
            
            if (vkEndCommandBuffer(gcb) != VK_SUCCESS)
            {
                knM_logFatal("Can't end graphics command buffer");
                return false;
            }
        }
    }

    VkSemaphoreCreateInfo sci2 = {};
    sci2.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fci = {};
    fci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fci.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    return true;
}

std::shared_ptr<Texture> RendererVK::loadTexture(const std::string& file)
{
    return nullptr;
}

std::shared_ptr<Font> RendererVK::loadFont(const std::string& file, size_t size)
{
    knM_logDebug("Loading font: " << file);

    return nullptr;
}

void RendererVK::draw(
    std::shared_ptr<Texture> t, 
    const Vec2f& position, 
    float angle, 
    const Vec2f& scale,
    const Vec2f& origin
)
{
    Sizei ts = t->getSize();

    draw(
        t, 
        position,
        angle,
        scale,
        origin,
        Recti { 0, 0, ts.x, ts.y },
        {}
    );
}

void RendererVK::draw(
    std::shared_ptr<Texture> t, 
    const Vec2f& position, 
    float angle, 
    const Vec2f& scale,
    const Vec2f& origin,
    const Recti& rect,
    const Colori& color
)
{
    assert(t);
}

void RendererVK::draw(
    const Rectf& rect,
    const Colori& color
)
{

}

void RendererVK::draw(
    const Vec2f& a,
    const Vec2f& b,
    const Colori& color
)
{

}

void RendererVK::clear()
{
    std::shared_ptr<vulkan::Fence> frameFence =
        this->device->getSwapchain()->getFrameCompletedFence(this->currentInFlightFrame)
    ;

    if (!frameFence->wait())
    {
        knM_logFatal("Can't wait for frame fence");
        return;
    }

    if (!frameFence->reset())
    {
        knM_logFatal("Can't reset fences");
        return;
    }

    uint32_t nextIdx = 0;

    if (!this->device->getSwapchain()->acquireNext(&nextIdx, this->currentInFlightFrame))
    {
        knM_logFatal("Can't acquire next swapchain image");
        return;
    }

    uint32_t nextCommandOffset = this->device->getSwapchain()->getResourceOffset(nextIdx, this->currentInFlightFrame);

    vulkan::Queue::Sync sync = {};
    sync.submitFence = frameFence;
    sync.waitFor = this->device->getSwapchain()->getImageReadySemaphore(this->currentInFlightFrame);
    sync.completeSignal = this->device->getSwapchain()->getRenderCompleteSemaphore(nextIdx, this->currentInFlightFrame);
    
    if (!this->graphQueue->submit(nextCommandOffset, sync))
    {
        knM_logFatal("Can't submit graphic queue");
        return;
    }

    if (!this->device->getSwapchain()->present(this->presQueue, nextIdx, sync.completeSignal))
    {
        knM_logFatal("Can't present swapchain image");
        return;
    }

    this->currentInFlightFrame = 
        (this->currentInFlightFrame + 1) % 
        this->device->getConfig().framesInFlight
    ;
}

void RendererVK::flip()
{
    assert(this->window);
}

void RendererVK::setCameraPosition(const Vec2f& pos)
{
    this->cameraPos = pos;
}

void RendererVK::setCameraZoom(float zoom)
{
    this->cameraZoom = zoom;
}

Vec2f RendererVK::getCameraPosition()
{
    return this->cameraPos;
}

float RendererVK::getCameraZoom()
{
    return this->cameraZoom;
}

Vec2i RendererVK::getResolution()
{
    assert(this->window);

    int w, h;
    glfwGetWindowSize(this->window, &w, &h);

    return { w, h };
}

} // namespace graphics
} // namespace kola