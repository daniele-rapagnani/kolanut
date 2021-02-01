#include "kolanut/graphics/vulkan/RendererVK.h"
#include "kolanut/graphics/vulkan/TextureVK.h"
#include "kolanut/graphics/vulkan/FontVK.h"
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

VkBool32 onVulkanValidationLayerMessage(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
)
{
    switch(messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            knM_logInfo("[Vulkan] " << pCallbackData->pMessage);
            break;

        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            knM_logWarning("[Vulkan] " << pCallbackData->pMessage);
            break;

        default:
            knM_logError("[Vulkan] " << pCallbackData->pMessage);
            break;
    }

    return VK_FALSE;
}

template <typename T>
VkResult getVulkanList(
    std::vector<T>& out,
    std::function<VkResult(uint32_t* count, T* list)> listFunc
)
{
    uint32_t count = 0;
    VkResult r = VK_SUCCESS;

    if ((r = listFunc(&count, nullptr)) != VK_SUCCESS)
    {
        return r;
    }

    out.clear();
    out.resize(count);

    if (count == 0)
    {
        return VK_SUCCESS;
    }

    if ((r = listFunc(&count, &out[0])) != VK_SUCCESS)
    {
        return r;
    }

    return VK_SUCCESS;
}

template <typename T>
std::vector<T> getVulkanList(
    std::function<void(uint32_t* count, T* list)> listFunc
)
{
    std::vector<T> out;
    
    getVulkanList<T>(out, [listFunc] (uint32_t* count, T* list) {
        listFunc(count, list);
        return VK_SUCCESS;
    });

    return out;
}

template <typename T>
std::vector<T> getVulkanListFatal(
    std::function<VkResult(uint32_t* count, T* list)> listFunc,
    const std::string& error
)
{
    std::vector<T> out;
    VkResult errorCode = getVulkanList<T>(out, listFunc);

    if (errorCode != VK_SUCCESS)
    {
        knM_logFatal(error << ". Code: " << errorCode);
    }

    return out;
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

    VkApplicationInfo ai = {};
    ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ai.pApplicationName = config.windowTitle.c_str();
    ai.applicationVersion = 1;
    ai.apiVersion = VK_API_VERSION_1_0;

    static const char* layers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    std::vector<const char*> extensions = {
        "VK_EXT_debug_utils",
        "VK_KHR_get_physical_device_properties2"
    };

    uint32_t glfwExtensionsCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

    std::copy(
        glfwExtensions, 
        glfwExtensions + glfwExtensionsCount, 
        std::back_inserter(extensions)
    );

    VkDebugUtilsMessengerCreateInfoEXT mci = {};
    mci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    mci.messageSeverity = (
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
    );

    mci.messageType = (
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
    );

    mci.pfnUserCallback = onVulkanValidationLayerMessage;

    VkInstanceCreateInfo ici = {};
    ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ici.pApplicationInfo = &ai;
    ici.enabledExtensionCount = extensions.size();
    ici.ppEnabledExtensionNames = &extensions[0];
    ici.enabledLayerCount = sizeof(layers) / sizeof(const char*);
    ici.ppEnabledLayerNames = layers;
    ici.pNext = &mci;

    if (vkCreateInstance(&ici, nullptr, &this->instance) != VK_SUCCESS)
    {
        knM_logFatal("Can't create Vulkan instance");
        return false;
    }

    auto createDebug = 
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            glfwGetInstanceProcAddress(this->instance, "vkCreateDebugUtilsMessengerEXT")
        )
    ;

    if (!createDebug)
    {
        knM_logFatal("vkCreateDebugUtilsMessengerEXT not supported");
        return false;
    }

    if (createDebug(this->instance, &mci, nullptr, &this->messenger) != VK_SUCCESS)
    {
        knM_logFatal("Can't create debug messenger");
        return false;
    }

    if (
        glfwCreateWindowSurface(this->instance, this->window, nullptr, &this->surface)
        != VK_SUCCESS
    )
    {
        knM_logFatal("Can't create Vulkan surface");
        return false;
    }

    uint32_t physicalDevicesCount = 0;
    
    if (vkEnumeratePhysicalDevices(this->instance, &physicalDevicesCount, nullptr) != VK_SUCCESS)
    {
        knM_logFatal("Can't get number of Vulkan physical devices");
        return false;
    }

    if (physicalDevicesCount == 0)
    {
        knM_logFatal("Can't find any Vulkan physical device");
        return false;
    }

    std::vector<VkPhysicalDevice> devices(physicalDevicesCount);

    if (vkEnumeratePhysicalDevices(this->instance, &physicalDevicesCount, &devices[0]) != VK_SUCCESS)
    {
        knM_logFatal("Can't enumerate Vulkan physical devices");
        return false;
    }

    this->physicalDevice = devices[0];

    VkPhysicalDeviceFeatures feature = {};
    vkGetPhysicalDeviceFeatures(this->physicalDevice, &feature);

    std::vector<VkQueueFamilyProperties> queuesProps = getVulkanList<VkQueueFamilyProperties>(
        std::bind(
            vkGetPhysicalDeviceQueueFamilyProperties,
            this->physicalDevice,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );

    int32_t presentationQueueFamilyIdx = -1;
    int32_t graphicsQueueFamilyIdx = -1;

    for (auto it = queuesProps.begin(); it != queuesProps.end(); ++it)
    {
        if (presentationQueueFamilyIdx >= 0 && graphicsQueueFamilyIdx >= 0)
        {
            break;
        }

        size_t idx = std::distance(queuesProps.begin(), it);

        if (graphicsQueueFamilyIdx < 0 && it->queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsQueueFamilyIdx = idx;
        }

        if (presentationQueueFamilyIdx < 0)
        {
            VkBool32 isSurfaceSupported = VK_FALSE;

            if (
                vkGetPhysicalDeviceSurfaceSupportKHR(
                    this->physicalDevice, 
                    idx, 
                    this->surface, 
                    &isSurfaceSupported
                ) != VK_SUCCESS
            )
            {
                knM_logError("Can't check for physical device surface support.");
            }
            else if (isSurfaceSupported == VK_TRUE)
            {
                presentationQueueFamilyIdx = idx;
            }
        }
    }

    std::vector<VkDeviceQueueCreateInfo> dqcis;

    float priority = 0.0f;
    
    VkDeviceQueueCreateInfo presentDqci = {};
    presentDqci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    presentDqci.queueCount = 1;
    presentDqci.queueFamilyIndex = presentationQueueFamilyIdx;
    presentDqci.pQueuePriorities = &priority;

    dqcis.push_back(presentDqci);

    if (presentationQueueFamilyIdx != graphicsQueueFamilyIdx)
    {
        VkDeviceQueueCreateInfo graphDqci = {};
        graphDqci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphDqci.queueCount = 1;
        graphDqci.queueFamilyIndex = graphicsQueueFamilyIdx;
        presentDqci.pQueuePriorities = &priority;

        dqcis.push_back(graphDqci);
    }

    std::vector<VkExtensionProperties> extProps = getVulkanListFatal<VkExtensionProperties>(
        std::bind(
            vkEnumerateDeviceExtensionProperties,
            this->physicalDevice,
            nullptr,
            std::placeholders::_1,
            std::placeholders::_2
        ),
        "Can't get device extensions"
    );

    std::vector<const char*> deviceExts = {};
    deviceExts.push_back("VK_KHR_swapchain");

    if (!extProps.empty())
    {
        auto ext = std::find_if(extProps.begin(), extProps.end(), [] (const VkExtensionProperties& p) {
            return strcmp(p.extensionName, "VK_KHR_portability_subset") == 0;
        });

        if (ext != extProps.end())
        {
            deviceExts.push_back("VK_KHR_portability_subset");
        }
    }

    VkDeviceCreateInfo dci = {};
    dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    dci.pQueueCreateInfos = &dqcis[0];
    dci.queueCreateInfoCount = dqcis.size();
    dci.enabledExtensionCount = deviceExts.size();
    dci.ppEnabledExtensionNames = &deviceExts[0];

    if (vkCreateDevice(this->physicalDevice, &dci, nullptr, &this->logicalDevice) != VK_SUCCESS)
    {
        knM_logFatal("Can't create Vulkan logical device");
        return false;
    }

    vkGetDeviceQueue(this->logicalDevice, presentationQueueFamilyIdx, 0, &this->presQueue);

    if (presentationQueueFamilyIdx != graphicsQueueFamilyIdx)
    {
        vkGetDeviceQueue(this->logicalDevice, graphicsQueueFamilyIdx, 0, &this->graphQueue);
    }
    else
    {
        this->graphQueue = this->presQueue;
    }

    VkFormatProperties fp = {};
    vkGetPhysicalDeviceFormatProperties(this->physicalDevice, VK_FORMAT_R8G8B8A8_SRGB, &fp);

    if (!fp.bufferFeatures)
    {
        knM_logFatal("RGBA SRGB is not supported");
        return false;
    }
    
    VkSurfaceCapabilitiesKHR sc = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->physicalDevice, this->surface, &sc);

    if (sc.minImageCount > 2 || sc.maxImageCount < 2)
    {
        knM_logFatal("Unsupported surface format: min or max image count is not enough");
        return false;
    }

    std::vector<uint32_t> queueFamilies;
    queueFamilies.push_back(presentationQueueFamilyIdx);
    
    if (presentationQueueFamilyIdx != graphicsQueueFamilyIdx)
    {
        queueFamilies.push_back(graphicsQueueFamilyIdx);
    }

    std::vector<VkSurfaceFormatKHR> surfaceForms = getVulkanList<VkSurfaceFormatKHR>(
        std::bind(
            vkGetPhysicalDeviceSurfaceFormatsKHR,
            this->physicalDevice,
            this->surface,
            std::placeholders::_1,
            std::placeholders::_2
        )
    );

    bool found = false;

    for (const auto& f : surfaceForms)
    {
        if (f.format == VK_FORMAT_B8G8R8A8_SRGB)
        {
            found = f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
            break;
        }
    }

    if (!found)
    {
        knM_logFatal("Can't find a suitable surface format");
        return false;
    }

    VkSwapchainCreateInfoKHR sci = {};
    sci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    sci.surface = this->surface;
    sci.minImageCount = 2;
    sci.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
    sci.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    sci.imageExtent = sc.currentExtent;
    sci.imageArrayLayers = 1;
    sci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    sci.imageSharingMode = queueFamilies.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE; // TODO: Family transfer
    sci.queueFamilyIndexCount = queueFamilies.size();
    sci.pQueueFamilyIndices = &queueFamilies[0];
    sci.preTransform = sc.currentTransform;
    sci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    sci.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    sci.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(this->logicalDevice, &sci, nullptr, &this->swapchain) != VK_SUCCESS)
    {
        knM_logFatal("Can't create swapchain");
        return false;
    }

    this->swapchainImages = getVulkanListFatal<VkImage>(
        std::bind(
            vkGetSwapchainImagesKHR,
            this->logicalDevice,
            this->swapchain,
            std::placeholders::_1,
            std::placeholders::_2
        ),
        "Can't get swapchain's image list"
    );

    this->swapchainImageViews.clear();

    for (VkImage i : this->swapchainImages)
    {
        VkImageViewCreateInfo ivci = {};
        ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ivci.image = i;
        ivci.format = sci.imageFormat;
        ivci.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ivci.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        ivci.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        this->swapchainImageViews.emplace_back();

        if (
            vkCreateImageView(
                this->logicalDevice, 
                &ivci, 
                nullptr, 
                &this->swapchainImageViews.back()
            ) != VK_SUCCESS
        )
        {
            knM_logFatal("Can't create image view for swapchain's image");
            return false;
        }
    }

    std::vector<char> buffer;

    if (!di::get<filesystem::FilesystemEngine>()->getFileContent("assets/mainv.svert", buffer))
    {
        knM_logFatal("Can't open main vertex shader");
        return false;
    }

    VkShaderModuleCreateInfo smci = {};
    smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    smci.codeSize = buffer.size();
    smci.pCode = reinterpret_cast<uint32_t*>(&buffer[0]);

    if (vkCreateShaderModule(this->logicalDevice, &smci, nullptr, &this->vertexShader) != VK_SUCCESS)
    {
        knM_logFatal("Can't create vertex shader");
        return false;
    }

    buffer.clear();

    if (!di::get<filesystem::FilesystemEngine>()->getFileContent("assets/mainv.sfrag", buffer))
    {
        knM_logFatal("Can't open main fragment shader");
        return false;
    }

    smci = {};
    smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    smci.codeSize = buffer.size();
    smci.pCode = reinterpret_cast<uint32_t*>(&buffer[0]);

    if (vkCreateShaderModule(this->logicalDevice, &smci, nullptr, &this->fragmentShader) != VK_SUCCESS)
    {
        knM_logFatal("Can't create fragment shader");
        return false;
    }

    VkAttachmentDescription atd = {};
    atd.format = sci.imageFormat;
    atd.samples = VK_SAMPLE_COUNT_1_BIT;
    atd.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    atd.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    atd.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    atd.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference atr = {};
    atr.attachment = 0;
    atr.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription sbd = {};
    sbd.colorAttachmentCount = 1;
    sbd.pColorAttachments = &atr;
    
    VkRenderPassCreateInfo rpci = {};
    rpci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rpci.attachmentCount = 1;
    rpci.pAttachments = &atd;
    rpci.subpassCount = 1;
    rpci.pSubpasses = &sbd;

    if (vkCreateRenderPass(this->logicalDevice, &rpci, nullptr, &this->renderPass) != VK_SUCCESS)
    {
        knM_logFatal("Can't create render pass");
        return false;
    }

    VkPipelineShaderStageCreateInfo pssci[2] = {};
    pssci[0].sType = pssci[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pssci[0].module = this->vertexShader;
    pssci[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    pssci[0].pName = "main";

    pssci[1].module = this->fragmentShader;
    pssci[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pssci[1].pName = "main";

    VkPipelineRasterizationStateCreateInfo prsci = {};
    prsci.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    prsci.rasterizerDiscardEnable = VK_FALSE;
    prsci.polygonMode = VK_POLYGON_MODE_FILL;
    prsci.cullMode = VK_CULL_MODE_NONE;
    prsci.frontFace = VK_FRONT_FACE_CLOCKWISE;
    prsci.lineWidth = 1.0f;

    VkPipelineInputAssemblyStateCreateInfo paci = {};
    paci.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    paci.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    paci.primitiveRestartEnable = VK_FALSE;

    VkPipelineVertexInputStateCreateInfo pvisci = {};
    pvisci.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineColorBlendAttachmentState pcbas = {};
    pcbas.blendEnable = VK_FALSE;
    pcbas.colorWriteMask = 
        VK_COLOR_COMPONENT_R_BIT 
        | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT
    ;

    VkPipelineColorBlendStateCreateInfo pcbsci = {};
    pcbsci.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pcbsci.logicOp = VK_LOGIC_OP_COPY;
    pcbsci.logicOpEnable = VK_FALSE;
    pcbsci.attachmentCount = 1;
    pcbsci.pAttachments = &pcbas;

    VkPipelineMultisampleStateCreateInfo pmsci = {};
    pmsci.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pmsci.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkViewport viewport = {};
    viewport.width = sc.currentExtent.width;
    viewport.height = sc.currentExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    VkRect2D scissoring = {};
    scissoring.extent = sc.currentExtent;

    VkPipelineViewportStateCreateInfo pvsci = {};
    pvsci.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pvsci.viewportCount = 1;
    pvsci.pViewports = &viewport;
    pvsci.scissorCount = 1;
    pvsci.pScissors = &scissoring;

    VkPipelineLayoutCreateInfo plci = {};
    plci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    
    if (vkCreatePipelineLayout(this->logicalDevice, &plci, nullptr, &this->pipelineLayout) != VK_SUCCESS)
    {
        knM_logFatal("Can' create pipeline layout");
        return false;
    }

    VkGraphicsPipelineCreateInfo gpci = {};
    gpci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    gpci.pStages = pssci;
    gpci.stageCount = 2;
    gpci.renderPass = this->renderPass;
    gpci.pRasterizationState = &prsci;
    gpci.layout = this->pipelineLayout;
    gpci.pInputAssemblyState = &paci;
    gpci.pVertexInputState = &pvisci;
    gpci.pColorBlendState = &pcbsci;
    gpci.pMultisampleState = &pmsci;
    gpci.pViewportState = &pvsci;

    if (
        vkCreateGraphicsPipelines(this->logicalDevice, VK_NULL_HANDLE, 1, &gpci, nullptr, &this->pipeline)
        != VK_SUCCESS
    )
    {
        knM_logFatal("Can't create pipeline");
        return false;
    }

    VkCommandPoolCreateInfo cpci = {};
    cpci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cpci.queueFamilyIndex = graphicsQueueFamilyIdx;

    if (vkCreateCommandPool(this->logicalDevice, &cpci, nullptr, &this->graphicsCmdPool) != VK_SUCCESS)
    {
        knM_logFatal("Can't create graphics command pool");
        return false;
    }

    cpci.queueFamilyIndex = presentationQueueFamilyIdx;

    if (vkCreateCommandPool(this->logicalDevice, &cpci, nullptr, &this->presentCmdPool) != VK_SUCCESS)
    {
        knM_logFatal("Can't create graphics command pool");
        return false;
    }

    VkCommandBufferAllocateInfo cbai = {};
    cbai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cbai.commandBufferCount = this->swapchainImageViews.size() * this->framesInFlight;
    cbai.commandPool = this->graphicsCmdPool;
    cbai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    this->graphicsCommandBuffers.resize(cbai.commandBufferCount);
    this->presentCommandBuffers.resize(cbai.commandBufferCount);

    if (vkAllocateCommandBuffers(this->logicalDevice, &cbai, &this->graphicsCommandBuffers[0]) != VK_SUCCESS)
    {
        knM_logFatal("Can't create graphics command buffer");
        return false;
    }

    cbai.commandPool = this->presentCmdPool;
    
    if (vkAllocateCommandBuffers(this->logicalDevice, &cbai, &this->presentCommandBuffers[0]) != VK_SUCCESS)
    {
        knM_logFatal("Can't create present command buffer");
        return false;
    }

    for (auto it = this->swapchainImageViews.cbegin(); it != this->swapchainImageViews.cend(); ++it)
    {
        for (uint8_t i = 0; i < this->framesInFlight; i++)
        {
            VkFramebufferCreateInfo fbci = {};
            fbci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbci.attachmentCount = 1;
            fbci.pAttachments = &(*it);
            fbci.renderPass = this->renderPass;
            fbci.layers = 1;
            fbci.width = sc.currentExtent.width;
            fbci.height = sc.currentExtent.height;

            this->framebuffers.emplace_back();

            if (
                vkCreateFramebuffer(this->logicalDevice, &fbci, nullptr, &this->framebuffers.back())
                != VK_SUCCESS
            )
            {
                knM_logFatal("Can't create framebuffer object");
                return false;
            }

            decltype(this->swapchainImageViews)::size_type idx = 
                std::distance(this->swapchainImageViews.cbegin(), it)
                + this->swapchainImageViews.size() * i
            ;

            VkCommandBuffer gcb = this->graphicsCommandBuffers[idx];
            VkCommandBuffer pcb = this->presentCommandBuffers[idx];
    
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
            rpbi.renderPass = this->renderPass;
            rpbi.framebuffer = this->framebuffers.back();
            rpbi.renderArea.extent = sc.currentExtent;
            rpbi.clearValueCount = 1;
            rpbi.pClearValues = &cv;

            vkCmdBeginRenderPass(gcb, &rpbi, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(gcb, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipeline);
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

    for (uint8_t i = 0; i < this->framesInFlight; i++)
    {
        this->swapchainImageReadySems.emplace_back();
        this->renderCompleteSems.emplace_back();
        this->frameCompleteFens.emplace_back();

        if (vkCreateSemaphore(this->logicalDevice, &sci2, nullptr, &this->swapchainImageReadySems.back()) != VK_SUCCESS)
        {
            knM_logFatal("Can't create swapchian semaphore");
            return false;
        }

        if (vkCreateSemaphore(this->logicalDevice, &sci2, nullptr, &this->renderCompleteSems.back()) != VK_SUCCESS)
        {
            knM_logFatal("Can't create render semaphore");
            return false;
        }

        if (vkCreateFence(this->logicalDevice, &fci, nullptr, &this->frameCompleteFens.back()) != VK_SUCCESS)
        {
            knM_logFatal("Can't create frame fence");
            return false;
        }
    }

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
    if (
        vkWaitForFences(
            this->logicalDevice, 
            1, 
            &this->frameCompleteFens[this->currentInFlightFrame], 
            VK_TRUE,
            std::numeric_limits<uint64_t>::max()
        ) != VK_SUCCESS
    )
    {
        knM_logFatal("Can't wait for frame fence");
        return;
    }

    if (
        vkResetFences(this->logicalDevice, 1, &this->frameCompleteFens[this->currentInFlightFrame])
        != VK_SUCCESS
    )
    {
        knM_logFatal("Can't reset fences");
        return;
    }

    uint32_t nextIdx = 0;

    if (
        vkAcquireNextImageKHR(
            this->logicalDevice, 
            this->swapchain, 
            std::numeric_limits<uint64_t>::max(),
            this->swapchainImageReadySems[this->currentInFlightFrame],
            VK_NULL_HANDLE,
            &nextIdx
        ) != VK_SUCCESS
    )
    {
        knM_logFatal("Can't acquire next swapchain image");
        return;
    }

    uint32_t nextCommandOffset = nextIdx + this->swapchainImageViews.size() * this->currentInFlightFrame;

    VkPipelineStageFlags waitMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo si = {};
    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    si.commandBufferCount = 1;
    si.pCommandBuffers = &this->graphicsCommandBuffers[nextCommandOffset];
    si.pWaitSemaphores = &this->swapchainImageReadySems[this->currentInFlightFrame];
    si.waitSemaphoreCount = 1;
    si.pWaitDstStageMask = &waitMask;
    si.signalSemaphoreCount = 1;
    si.pSignalSemaphores = &this->renderCompleteSems[this->currentInFlightFrame];
    
    if (vkQueueSubmit(this->graphQueue, 1, &si, this->frameCompleteFens[this->currentInFlightFrame]) != VK_SUCCESS)
    {
        knM_logFatal("Can't submit graphic queue");
        return;
    }

    VkPresentInfoKHR pi = {};
    pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    pi.swapchainCount = 1;
    pi.pSwapchains = &this->swapchain;
    pi.pImageIndices = &nextIdx;
    pi.waitSemaphoreCount = 1;
    pi.pWaitSemaphores = &this->renderCompleteSems[this->currentInFlightFrame];

    if (vkQueuePresentKHR(this->presQueue, &pi) != VK_SUCCESS)
    {
        knM_logFatal("Can't present swapchain image");
        return;
    }

    this->currentInFlightFrame = (this->currentInFlightFrame + 1) % this->framesInFlight;
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