#include "kolanut/graphics/vulkan/RendererVK.h"
#include "kolanut/graphics/vulkan/TextureVK.h"
#include "kolanut/graphics/vulkan/FontVK.h"
#include "kolanut/graphics/vulkan/ShaderVK.h"
#include "kolanut/graphics/vulkan/ProgramVK.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"
#include "kolanut/graphics/vulkan/utils/MappedMemory.h"
#include "kolanut/graphics/Vertex.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/stats/Stats.h"
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

namespace {

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
    setUseNoAPI(true);
    
    if (!RendererGLFW::doInit(config))
    {
        return false;
    }

    knM_logDebug("Initilizing Vulkan renderer");

    uint32_t glfwExtensionsCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

    vulkan::Instance::Config instConf = {};
    instConf.appName = config.windowTitle;

    if (vulkan::Instance::isExtensionSupported("VK_KHR_get_physical_device_properties2"))
    {
        instConf.extensions.push_back("VK_KHR_get_physical_device_properties2");
    }

    if (getConfig().enableAPIDebug)
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
        glfwCreateWindowSurface(this->instance->getVkHandle(), getWindow(), nullptr, &this->surface) 
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

    if (!getConfig().forceGPU.empty())
    {
        for (const auto& dev : vulkan::PhysicalDevice::getDevices(this->instance))
        {
            knM_logInfo("Available GPU: " << dev->getName());

            if (dev->getName() == getConfig().forceGPU)
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

    devConfig.framesInFlight = getConfig().framesInFlight;
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

    if (
        !this->graphQueue->createCommandBuffers(
            this->graphCommandBuffers, 
            this->device->getConfig().framesInFlight
        )
    )
    {
        knM_logFatal("Can't create graphic command buffers");
        return false;
    }

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
        vulkan::UniformsBuffer::Config c = {};
        c.maxSets = MAX_DESC_SETS;

        this->uniformsBuffer = vulkan::make_init_fatal<vulkan::UniformsBuffer>(this->device, c);
    }

    this->device->getSwapchain()->initFramebuffers(this->device->getRenderPasses().back());

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

void RendererVK::drawSurface(const DrawSurface& req)
{
    std::shared_ptr<ProgramVK> program = 
        std::static_pointer_cast<ProgramVK>(req.program)
    ;

    std::shared_ptr<TextureVK> texture = 
        std::static_pointer_cast<TextureVK>(req.texture)
    ;

    ViewUniform vu = {};
    vu.screenSize.x = getDesignResolution().x;
    vu.screenSize.y = getDesignResolution().y;
    vu.camera = req.camera;
    vu.transform = req.transform;

    vulkan::UniformsBuffer::Handle uh = 
        this->uniformsBuffer->addUniform(&vu, sizeof(vu), getCurrentFrame())
    ;

    auto ds = this->descriptorPool->allocateSet(program->getVKPipeline()->descriptorSetLayout);
    assert(ds);

    this->descriptorSets.push_back(ds);

    texture->getTexture()->use(this->sampler, ds, 1);
    this->uniformsBuffer->bind(uh, ds, 2);

    std::shared_ptr<vulkan::CommandBuffer> gcb = this->graphCommandBuffers[getCurrentFrame()];
    auto h = reinterpret_cast<GeometryBuffer::Handle>(req.vertices);

    {
        TracyVkZone(
            this->tracyContextes[getCurrentFrame()],
            gcb->getVkHandle(),
            "Draw"
        )

        VkCommandBuffer b = gcb->getVkHandle();
        vkCmdBindPipeline(b, VK_PIPELINE_BIND_POINT_GRAPHICS, program->getVKPipeline()->getVkHandle());
        ds->bind(b, program->getVKPipeline()->layout, VK_PIPELINE_BIND_POINT_GRAPHICS);
        std::static_pointer_cast<GeometryBufferVK>(getGeometryBuffer())->bind(h, gcb);
        vkCmdDraw(b, req.verticesCount, 1, 0, 0);
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

void RendererVK::doClear()
{
    this->descriptorPool->reset();
    this->descriptorSets.clear();
    this->uniformsBuffer->reset(getCurrentFrame());

    std::shared_ptr<vulkan::Fence> frameFence = this->frameCompletedFence[getCurrentFrame()];

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
            this->imageReadySemaphores[getCurrentFrame()]
        )
    )
    {
        knM_logFatal("Can't acquire next swapchain image");
        return;
    }

    std::shared_ptr<vulkan::CommandBuffer> gcb = this->graphCommandBuffers[getCurrentFrame()];
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
        getCurrentFrame() * 2
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
        getCurrentFrame()
    );

    rpbi.renderArea.extent = sc.currentExtent;
    rpbi.clearValueCount = 1;
    rpbi.pClearValues = &cv;

    assert(rpbi.framebuffer);

    vkCmdBeginRenderPass(b, &rpbi, VK_SUBPASS_CONTENTS_INLINE);
}

void RendererVK::doFlip()
{
    std::shared_ptr<vulkan::CommandBuffer> gcb = this->graphCommandBuffers[getCurrentFrame()];

    vkCmdEndRenderPass(gcb->getVkHandle());

    size_t qIdx = getCurrentFrame() * 2;

    vkCmdWriteTimestamp(
        gcb->getVkHandle(), 
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
        this->queryPool->getVkHandle(), 
        qIdx + 1
    );

    TracyVkCollect(
        this->tracyContextes[getCurrentFrame()],
        gcb->getVkHandle()
    )

    gcb->end();

    vulkan::Queue::Sync sync = {};
    sync.submitFence = this->frameCompletedFence[getCurrentFrame()];
    sync.waitFor = this->imageReadySemaphores[getCurrentFrame()];
    sync.completeSignal = this->renderCompleteSemaphores[getCurrentFrame()];
    
    if (!this->graphQueue->submit(this->graphCommandBuffers[getCurrentFrame()], sync))
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

    auto stats = di::get<stats::StatsEngine>();
    stats->addSample(stats::StatsEngine::Measure::GPU_TIME, elapsedMs);
}

Vec2i RendererVK::getPixelResolution() const
{
    assert(this->physicalDevice);
    assert(this->device);

    auto sc =
        this->physicalDevice->getSurfaceCapabilities(this->device->getSwapchain()->getSurface())
    ;

    return { sc.currentExtent.width, sc.currentExtent.height };
}

std::shared_ptr<Texture> RendererVK::createTexture()
{
    return std::make_shared<TextureVK>();
}

std::shared_ptr<Font> RendererVK::createFont()
{
    return std::make_shared<FontVK>();
}

std::shared_ptr<Shader> RendererVK::createShader()
{
    assert(this->device);

    auto shader = std::make_shared<ShaderVK>();
    shader->setDevice(this->device);
    return shader;
}

std::shared_ptr<Program> RendererVK::createProgram()
{
    assert(this->surface);
    assert(this->device);

    auto program = std::make_shared<ProgramVK>();

    VkSurfaceCapabilitiesKHR sc = 
        getDevice()->getPhysicalDevice()->getSurfaceCapabilities(this->surface)
    ;

    program->setDevice(this->device);
    program->setViewportSize({ sc.currentExtent.width, sc.currentExtent.height });
    return program;
}

std::shared_ptr<GeometryBuffer> RendererVK::createGeometryBuffer()
{
    assert(this->device);
    auto geo = std::make_shared<GeometryBufferVK>();
    geo->setDevice(this->device);
    return geo;
}

} // namespace graphics
} // namespace kola