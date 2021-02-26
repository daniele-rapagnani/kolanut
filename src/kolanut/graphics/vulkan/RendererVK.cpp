#include "kolanut/graphics/vulkan/RendererVK.h"
#include "kolanut/graphics/vulkan/TextureVK.h"
#include "kolanut/graphics/vulkan/FontVK.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"
#include "kolanut/graphics/vulkan/utils/MappedMemory.h"
#include "kolanut/graphics/vulkan/utils/Vertex.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

#include <cstring>
#include <sstream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <functional>
#include <limits>
#include <cmath>

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

struct ViewUniform
{
    Vec2f screenSize = {};
    alignas(16) glm::mat4 transform { 1.0f };
    alignas(16) glm::mat4 camera { 1.0f };
};

} // namespace 

bool RendererVK::doInit(const Config& config)
{
    this->config = config;

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

    if (vulkan::Instance::isExtensionSupported("VK_KHR_get_physical_device_properties2"))
    {
        instConf.extensions.push_back("VK_KHR_get_physical_device_properties2");
    }

    if (this->config.enableAPIDebug)
    {
        instConf.layers.push_back("VK_LAYER_KHRONOS_validation");
        instConf.extensions.push_back("VK_EXT_debug_utils");
        instConf.messengerCb = onVulkanValidationLayerMessage;
    }

    std::copy(
        glfwExtensions, 
        glfwExtensions + glfwExtensionsCount, 
        std::back_inserter(instConf.extensions)
    );

    this->instance = vulkan::make_init_fatal<vulkan::Instance>(instConf);
    assert(this->instance);

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

    if (!this->config.forceGPU.empty())
    {
        for (const auto& dev : vulkan::PhysicalDevice::getDevices(this->instance))
        {
            knM_logInfo("Available GPU: " << dev->getName());

            if (dev->getName() == this->config.forceGPU)
            {
                this->physicalDevice = dev;
                break;
            }
        }
    }

    if (!this->physicalDevice)
    {
        knM_logInfo("No specific GPU requested (or found), falling back to default.");
        this->physicalDevice = vulkan::PhysicalDevice::getDevices(this->instance)[0];
    }

    knM_logDebug("Using GPU: " << this->physicalDevice->getName());

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

    devConfig.framesInFlight = this->config.framesInFlight;
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

    this->device = vulkan::make_init_fatal<vulkan::Device>(this->physicalDevice,this->surface, devConfig);
    assert(this->device);

    this->presQueue = this->device->getQueue(presentationQueueFamilyIdx);
    this->graphQueue = (presentationQueueFamilyIdx != graphicsQueueFamilyIdx)
        ? this->device->getQueue(graphicsQueueFamilyIdx)
        : this->presQueue
    ;

    if (!
        this->graphQueue->createCommandBuffers(
            this->graphCommandBuffers, 
            this->device->getConfig().framesInFlight
        )
    )
    {
        knM_logFatal("Can't create graphic command buffers");
        return false;
    }

    this->vertexShader = vulkan::make_init_fatal<vulkan::ShaderModule>(
        this->device,
        di::get<filesystem::FilesystemEngine>(),
        "assets/mainv.svert"
    );

    assert(this->vertexShader);

    this->fragmentShader = vulkan::make_init_fatal<vulkan::ShaderModule>(
        this->device,
        di::get<filesystem::FilesystemEngine>(),
        "assets/mainv.sfrag"
    );

    assert(this->fragmentShader);

    VkSamplerCreateInfo sci = {};
    sci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sci.minFilter = VK_FILTER_NEAREST;
    sci.magFilter = VK_FILTER_NEAREST;
    sci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    sci.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sci.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sci.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sci.mipLodBias = 0.0f;
    sci.anisotropyEnable = VK_FALSE;
    sci.compareEnable = VK_FALSE;
    sci.compareOp = VK_COMPARE_OP_ALWAYS;
    sci.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    sci.unnormalizedCoordinates = VK_FALSE;

    if (vkCreateSampler(this->device->getVkHandle(), &sci, nullptr, &this->sampler) != VK_SUCCESS)
    {
        knM_logFatal("Can't create texture sampler");
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

    {
        vulkan::GeometryBuffer::Config c = {};
        c.maxFrames = this->device->getConfig().framesInFlight;
        c.maxVerts = this->config.maxGeometryBufferVertices;
        this->geomBuffer = vulkan::make_init_fatal<vulkan::GeometryBuffer>(this->device, c);
    }

    {
        vulkan::UniformsBuffer::Config c = {};
        c.maxSets = MAX_DESC_SETS;

        this->uniformsBuffer = vulkan::make_init_fatal<vulkan::UniformsBuffer>(this->device, c);
    }

    this->device->getSwapchain()->initFramebuffers(this->device->getRenderPasses().back());

    VkSurfaceCapabilitiesKHR sc = this->physicalDevice->getSurfaceCapabilities(this->surface);

    vulkan::Pipeline2D::Config p2dConf = {};
    p2dConf.fragmentShader = this->fragmentShader;
    p2dConf.vertexShader = this->vertexShader;
    p2dConf.renderPass = this->device->getRenderPasses().back();
    p2dConf.viewportWidth = sc.currentExtent.width;
    p2dConf.viewportHeight = sc.currentExtent.height;

    this->pipeline = vulkan::make_init_fatal<vulkan::Pipeline2D>(this->device, p2dConf);

    {
        vulkan::DescriptorPool::Config poolConfig = {};
        poolConfig.maxSets = MAX_DESC_SETS * this->device->getConfig().framesInFlight;

        poolConfig.sizes.push_back({ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_DESC_UNIFORM_BUFFERS });
        poolConfig.sizes.push_back({ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_DESC_IMAGE_SAMPLERS });

        this->descriptorPool = vulkan::make_init_fatal<vulkan::DescriptorPool>(this->device, poolConfig);
    }

    for (size_t frame = 0; frame < device->getConfig().framesInFlight; frame++)
    {
        this->imageReadySemaphores.push_back(vulkan::make_init_fatal<vulkan::Semaphore>(device));
        this->frameCompletedFence.push_back(vulkan::make_init_fatal<vulkan::Fence>(device, true));
        this->renderCompleteSemaphores.push_back(vulkan::make_init_fatal<vulkan::Semaphore>(device));
    }

    {
        vulkan::QueryPool::Config config = {};
        config.count = this->device->getConfig().framesInFlight * 2;
        config.type = VK_QUERY_TYPE_TIMESTAMP;
        
        this->queryPool = vulkan::make_init_fatal<vulkan::QueryPool>(this->device, config);
        this->gpuElapsedTimes.resize(config.count);
    }

    for (size_t i = 0; i < this->graphCommandBuffers.size(); i++)
    {
        TracyVkCtx ctx = TracyVkContext(
            this->physicalDevice->getVkHandle(),
            this->device->getVkHandle(),
            this->graphQueue->getVkHandle(),
            this->graphCommandBuffers[i]->getVkHandle()
        );

        this->tracyContextes.push_back(ctx);
    }

    return true;
}

std::shared_ptr<Texture> RendererVK::loadTexture(const std::string& file)
{
    knM_logDebug("Loading texture: " << file);
    
    std::shared_ptr<TextureVK> texture = std::make_shared<TextureVK>();
    
    if (!texture->load(file))
    {
        return nullptr;
    }

    return std::static_pointer_cast<Texture>(texture);
}

std::shared_ptr<Font> RendererVK::loadFont(const std::string& file, size_t size)
{
    knM_logDebug("Loading font: " << file);

    std::shared_ptr<FontVK> font = std::make_shared<FontVK>();
    
    if (!font->load(file, size))
    {
        return nullptr;
    }

    return std::static_pointer_cast<Font>(font);
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

    auto ds = this->descriptorPool->allocateSet(this->pipeline->descriptorSetLayout);
    assert(ds);

    this->descriptorSets.push_back(ds);

    std::shared_ptr<TextureVK> vkt = std::static_pointer_cast<TextureVK>(t);

    Sizei s = vkt->getSize();

    Rectf tcRect = { 
        rect.x / static_cast<float>(s.x), 
        rect.y / static_cast<float>(s.y),
        0.0f,
        0.0f
    };
    
    tcRect.z = tcRect.x + (rect.z / static_cast<float>(s.x));
    tcRect.w = tcRect.y + (rect.w / static_cast<float>(s.y));

    std::vector<vulkan::Vertex> vertices = {
        { Vec2f { 0.0f, 0.0f }, { tcRect.x, tcRect.y }, color },
        { Vec2f { 0.0f, rect.w }, { tcRect.x, tcRect.w }, color },
        { Vec2f { rect.z, 0.0f }, { tcRect.z, tcRect.y }, color },
        { Vec2f { rect.z, 0.0f }, { tcRect.z, tcRect.y }, color },
        { Vec2f { 0.0f, rect.w }, { tcRect.x, tcRect.w }, color },
        { Vec2f { rect.z, rect.w }, { tcRect.z, tcRect.w }, color },
    };

    vulkan::GeometryBuffer::Handle h = this->geomBuffer->addVertices(vertices, this->currentInFlightFrame);

    ViewUniform vu = {};
    vu.screenSize.x = getDesignResolution().x;
    vu.screenSize.y = getDesignResolution().y;

    // T * S * R * O, outer comes first
    vu.transform = glm::translate(
        glm::rotate(
            glm::scale(
                glm::translate(
                    vu.transform,
                    glm::vec3 { position.x, position.y, 0.0f }
                ),
                glm::vec3 { scale.x, scale.y, 0.0f }
            ),
            static_cast<float>(angle * (M_PI / 180.0)),
            glm::vec3 { 0.0f, 0.0f, 1.0f }
        ),
        glm::vec3 { -origin.x, -origin.y, 0.0f }
    );

    float resScale = getResolution().x / getDesignResolution().x;

    vu.camera = glm::translate(
        glm::scale(
            vu.camera,
            glm::vec3 {
                resScale * this->cameraZoom,
                resScale * this->cameraZoom,
                1.0f
            }
        ),
        glm::vec3 {
            -this->cameraPos.x, 
            -this->cameraPos.y,
            0.0f
        }
    );

    vulkan::UniformsBuffer::Handle uh = 
        this->uniformsBuffer->addUniform(&vu, sizeof(vu), this->currentInFlightFrame)
    ;

    vkt->getTexture()->use(this->sampler, ds, 1);
    this->uniformsBuffer->bind(uh, ds, 2);

    std::shared_ptr<vulkan::CommandBuffer> gcb = this->graphCommandBuffers[this->currentInFlightFrame];
    
    {
        TracyVkZone(
            this->tracyContextes[this->currentInFlightFrame],
            gcb->getVkHandle(),
            "Draw"
        )

        VkCommandBuffer b = gcb->getVkHandle();
        vkCmdBindPipeline(b, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline->getVkHandle());
        ds->bind(b, this->pipeline->layout, VK_PIPELINE_BIND_POINT_GRAPHICS);
        this->geomBuffer->bind(h, gcb);
        vkCmdDraw(b, vertices.size(), 1, 0, 0);
    }
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
    this->descriptorPool->reset();
    this->descriptorSets.clear();
    this->geomBuffer->reset(this->currentInFlightFrame);
    this->uniformsBuffer->reset(this->currentInFlightFrame);

    std::shared_ptr<vulkan::Fence> frameFence = this->frameCompletedFence[this->currentInFlightFrame];

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

    if (
        !this->device->getSwapchain()->acquireNext(
            &this->nextImageIdx, 
            this->imageReadySemaphores[this->currentInFlightFrame]
        )
    )
    {
        knM_logFatal("Can't acquire next swapchain image");
        return;
    }

    std::shared_ptr<vulkan::CommandBuffer> gcb = this->graphCommandBuffers[this->currentInFlightFrame];
    gcb->begin();
    VkCommandBuffer b = gcb->getVkHandle();

    vkCmdResetQueryPool(
        b, 
        this->queryPool->getVkHandle(), 
        0, 
        this->queryPool->getConfig().count 
    );

    vkCmdWriteTimestamp(
        b, 
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
        this->queryPool->getVkHandle(), 
        this->currentInFlightFrame * 2
    );

    VkClearValue cv = {};
    cv.color.float32[0] = 0.0f;
    cv.color.float32[1] = 0.0f;
    cv.color.float32[2] = 0.0f;
    cv.color.float32[3] = 1.0f;

    VkSurfaceCapabilitiesKHR sc = this->physicalDevice->getSurfaceCapabilities(this->surface);

    VkRenderPassBeginInfo rpbi = {};
    rpbi.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpbi.renderPass = this->device->getRenderPasses().back()->getVkHandle();
    rpbi.framebuffer = this->device->getSwapchain()->getFramebuffer(
        this->nextImageIdx, 
        this->currentInFlightFrame
    );

    rpbi.renderArea.extent = sc.currentExtent;
    rpbi.clearValueCount = 1;
    rpbi.pClearValues = &cv;

    assert(rpbi.framebuffer);

    vkCmdBeginRenderPass(b, &rpbi, VK_SUBPASS_CONTENTS_INLINE);
}

void RendererVK::flip()
{
    std::shared_ptr<vulkan::CommandBuffer> gcb = this->graphCommandBuffers[this->currentInFlightFrame];

    vkCmdEndRenderPass(gcb->getVkHandle());

    size_t qIdx = this->currentInFlightFrame * 2;

    vkCmdWriteTimestamp(
        gcb->getVkHandle(), 
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
        this->queryPool->getVkHandle(), 
        qIdx + 1
    );

    TracyVkCollect(
        this->tracyContextes[this->currentInFlightFrame],
        gcb->getVkHandle()
    )

    gcb->end();

    vulkan::Queue::Sync sync = {};
    sync.submitFence = this->frameCompletedFence[this->currentInFlightFrame];
    sync.waitFor = this->imageReadySemaphores[this->currentInFlightFrame];
    sync.completeSignal = this->renderCompleteSemaphores[this->currentInFlightFrame];
    
    if (!this->graphQueue->submit(this->graphCommandBuffers[this->currentInFlightFrame], sync))
    {
        knM_logFatal("Can't submit graphic queue");
        return;
    }

    if (!this->device->getSwapchain()->present(this->presQueue, this->nextImageIdx, sync.completeSignal))
    {
        knM_logFatal("Can't present swapchain image");
        return;
    }

    if (vkGetQueryPoolResults(
        this->device->getVkHandle(), 
        this->queryPool->getVkHandle(), 
        qIdx,
        2, 
        this->gpuElapsedTimes.size() * sizeof(uint64_t),
        &this->gpuElapsedTimes[qIdx], 
        sizeof(uint64_t), 
        VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT
    ) != VK_SUCCESS)
    {
        knM_logError("Can't retrieve timestamp");
    }

    uint64_t start = this->gpuElapsedTimes[qIdx];
    uint64_t end = this->gpuElapsedTimes[qIdx + 1];
    uint64_t diff = (end - start);

    double interval = 1000000.0 / this->physicalDevice->getProperties().limits.timestampPeriod;
    double elapsedMs = static_cast<double>(diff) / interval;

    this->gpuAvgTime += elapsedMs;
    this->gpuSamples++;

    if (this->gpuSamples == 60)
    {
        knM_logDebug(
            "[Vulkan] rendering took an avg of " 
            << (this->gpuAvgTime / this->gpuSamples) 
            << " ms for 60 frames"
        );

        this->gpuSamples = 0;
        this->gpuAvgTime = 0.0f;
    }

    this->currentInFlightFrame = 
        (this->currentInFlightFrame + 1) % 
        this->device->getConfig().framesInFlight
    ;
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

Vec2i RendererVK::getPixelResolution()
{
    assert(this->physicalDevice);
    assert(this->device);

    auto sc =
        this->physicalDevice->getSurfaceCapabilities(this->device->getSwapchain()->getSurface())
    ;

    return { sc.currentExtent.width, sc.currentExtent.height };
}

float RendererVK::getPixelsPerPoint()
{
    Vec2i r = getResolution();
    Vec2i pr = getPixelResolution();

    return pr.x / r.x;
}

} // namespace graphics
} // namespace kola