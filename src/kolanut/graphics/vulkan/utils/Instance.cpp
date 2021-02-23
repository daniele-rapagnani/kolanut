#include "kolanut/graphics/vulkan/utils/Instance.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"
#include "kolanut/core/Logging.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cassert>

namespace kola {
namespace graphics {
namespace vulkan {

namespace {

VKAPI_ATTR VkBool32 VKAPI_CALL onVulkanValidationLayerMessage(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
)
{
    if (!pUserData)
    {
        return VK_FALSE;
    }

    Instance* i = reinterpret_cast<Instance*>(pUserData);
    
    if (i->getConfig().messengerCb)
    {
        i->getConfig().messengerCb(
            messageSeverity,
            messageTypes,
            pCallbackData->pMessage
        );
    }

    return VK_FALSE;
}

} // namespace

std::vector<VkExtensionProperties> Instance::supportedExtensions = {};

const std::vector<VkExtensionProperties>& Instance::getSupportedExtensions()
{
    if (Instance::supportedExtensions.empty())
    {
        Instance::supportedExtensions = getVulkanListFatal<VkExtensionProperties>(
            std::bind(
                vkEnumerateInstanceExtensionProperties,
                nullptr,
                std::placeholders::_1,
                std::placeholders::_2
            ),
            "Can't enumerate supported instance extensions"
        );
    }

    return Instance::supportedExtensions;
}

bool Instance::isExtensionSupported(const std::string& extension)
{
    auto sp = getSupportedExtensions();

    return std::find_if(sp.begin(), sp.end(), [extension] (VkExtensionProperties ep) {
        return extension == ep.extensionName;
    }) != sp.end();
}

Instance::~Instance()
{
    if (this->messenger != VK_NULL_HANDLE)
    {
        auto destroyDebug = 
            reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                glfwGetInstanceProcAddress(getVkHandle(), "vkDestroyDebugUtilsMessengerEXT")
            )
        ;

        assert(destroyDebug);

        destroyDebug(getVkHandle(), this->messenger, nullptr);
    }

    vkDestroyInstance(getVkHandle(), nullptr);
}

bool Instance::init(const Instance::Config& config)
{
    this->config = config;

    for (const std::string& extName : config.extensions)
    {
        if (!Instance::isExtensionSupported(extName))
        {
            knM_logError("Instance extension '" << extName << "' is not supported");
            return false;
        }
    }

    VkDebugUtilsMessengerCreateInfoEXT mci = {};
    
    VkInstanceCreateInfo ici = {};
    ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    if (this->config.messengerCb)
    {
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
        ici.pNext = &mci;
    }

    VkApplicationInfo ai = {};
    ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ai.pApplicationName = config.appName.c_str();
    ai.applicationVersion = 1;
    ai.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char*> ccExtensions = toConstCharVec(config.extensions);
    std::vector<const char*> ccLayers = toConstCharVec(config.layers);

    ici.pApplicationInfo = &ai;
    ici.enabledExtensionCount = ccExtensions.size();
    ici.ppEnabledExtensionNames = &ccExtensions[0];
    ici.enabledLayerCount = ccLayers.size();
    ici.ppEnabledLayerNames = &ccLayers[0];

    if (vkCreateInstance(&ici, nullptr, &this->handle) != VK_SUCCESS)
    {
        knM_logError("Can't create vulkan instance");
        return false;
    }

    if (this->config.messengerCb)
    {
        auto createDebug = 
            reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                glfwGetInstanceProcAddress(getVkHandle(), "vkCreateDebugUtilsMessengerEXT")
            )
        ;

        if (!createDebug)
        {
            knM_logError("vkCreateDebugUtilsMessengerEXT not supported");
            return false;
        }

        if (createDebug(getVkHandle(), &mci, nullptr, &this->messenger) != VK_SUCCESS)
        {
            knM_logError("Can't create debug messenger");
            return false;
        }
    }

    return true;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola