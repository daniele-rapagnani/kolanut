#include "kolanut/graphics/vulkan/utils/PhysicalDevice.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"

#include <cassert>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace kola {
namespace graphics {
namespace vulkan {

std::vector<std::shared_ptr<PhysicalDevice>> PhysicalDevice::devices = {};

const std::vector<std::shared_ptr<PhysicalDevice>>& 
    PhysicalDevice::getDevices(std::shared_ptr<Instance> instance)
{
    if (PhysicalDevice::devices.empty())
    {
        std::vector<VkPhysicalDevice> devs = getVulkanListFatal<VkPhysicalDevice>(
            std::bind(
                vkEnumeratePhysicalDevices,
                instance->getVkHandle(),
                std::placeholders::_1,
                std::placeholders::_2
            ),
            "Can't enumerate physical GPUs"
        );

        for (VkPhysicalDevice pd : devs)
        {
            std::shared_ptr<PhysicalDevice> pdObj = std::make_shared<PhysicalDevice>(pd);
            PhysicalDevice::devices.push_back(pdObj);
        }
    }

    return PhysicalDevice::devices;
}

PhysicalDevice::PhysicalDevice(VkPhysicalDevice handle)
{
    assert(handle != VK_NULL_HANDLE);

    this->handle = handle;

    vkGetPhysicalDeviceFeatures(getVkHandle(), &this->features);
    vkGetPhysicalDeviceProperties(getVkHandle(), &this->properties);

    this->queueFamilies = vulkan::getVulkanList<VkQueueFamilyProperties>(
        std::bind(
            vkGetPhysicalDeviceQueueFamilyProperties,
            getVkHandle(),
            std::placeholders::_1,
            std::placeholders::_2
        )
    );
}

namespace {

std::unordered_map<
    QueueFamilyType, 
    std::function<bool(VkQueueFamilyProperties)>,
    EnumClassHash
> queueFamilyDetectors = {
    {
        QueueFamilyType::GRAPHICS,
        [] (VkQueueFamilyProperties p) {
            return ((p.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT);
        }
    },
    {
        QueueFamilyType::TRANSFER,
        [] (VkQueueFamilyProperties p) {
            return ((p.queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT);
        }
    }
};

} // namespace

const std::vector<VkExtensionProperties>& PhysicalDevice::getSupportedExtensions() const
{
    if (this->supportedExtensions.empty())
    {
        this->supportedExtensions = getVulkanListFatal<VkExtensionProperties>(
            std::bind(
                vkEnumerateDeviceExtensionProperties,
                this->getVkHandle(),
                nullptr,
                std::placeholders::_1,
                std::placeholders::_2
            ),
            "Can't enumerate supported instance extensions"
        );
    }

    return this->supportedExtensions;
}

bool PhysicalDevice::isExtensionSupported(const std::string& extension) const
{
    auto sp = getSupportedExtensions();

    return std::find_if(sp.begin(), sp.end(), [extension] (VkExtensionProperties ep) {
        return extension == ep.extensionName;
    }) != sp.end();
}

std::string PhysicalDevice::getName() const
{
    return this->properties.deviceName;
}

QueueFamily PhysicalDevice::getQueueFamily(QueueFamilyType type) const
{
    assert(queueFamilyDetectors.find(type) != queueFamilyDetectors.end());

    QueueFamily qf = {};

    for (auto it = this->queueFamilies.begin(); it != this->queueFamilies.end(); ++it)
    {
        if (queueFamilyDetectors[type](*it))
        {
            qf.index = std::distance(this->queueFamilies.begin(), it);
            qf.type = type;

            break;
        }
    }

    return qf;
}

QueueFamily PhysicalDevice::getPresentationQueueFamily(VkSurfaceKHR surface) const
{
    QueueFamily qf = {};

    if (!this->hasSwapchainSupport())
    {
        return qf;
    }

    for (auto it = this->queueFamilies.begin(); it != this->queueFamilies.end(); ++it)
    {
        size_t i = std::distance(this->queueFamilies.begin(), it);

        VkBool32 isSurfaceSupported = VK_FALSE;

        if (
            vkGetPhysicalDeviceSurfaceSupportKHR(
                getVkHandle(), 
                i, 
                surface, 
                &isSurfaceSupported
            ) != VK_SUCCESS
        )
        {
            knM_logError("Can't check for physical device surface support on queue: " << i);
        }
        else if (isSurfaceSupported == VK_TRUE)
        {
            qf.index = i;
            qf.type = QueueFamilyType::PRESENTATION;

            break;
        }
    }

    return qf;
}

VkFormatProperties PhysicalDevice::getFormatProperties(VkFormat format) const
{
    VkFormatProperties fp = {};
    vkGetPhysicalDeviceFormatProperties(getVkHandle(), format, &fp);

    return fp;
}

VkSurfaceCapabilitiesKHR PhysicalDevice::getSurfaceCapabilities(VkSurfaceKHR surface) const
{
    if (!this->hasSwapchainSupport())
    {
        return {};
    }

    VkSurfaceCapabilitiesKHR sc = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getVkHandle(), surface, &sc);

    return sc;
}

const std::vector<VkSurfaceFormatKHR>& PhysicalDevice::getSurfaceFormats(VkSurfaceKHR surface) const
{
    if (!this->hasSwapchainSupport())
    {
        return this->surfaceFormats;
    }

    if (this->surfaceFormats.empty())
    {
        this->surfaceFormats = vulkan::getVulkanList<VkSurfaceFormatKHR>(
            std::bind(
                vkGetPhysicalDeviceSurfaceFormatsKHR,
                getVkHandle(),
                surface,
                std::placeholders::_1,
                std::placeholders::_2
            )
        );
    }

    return this->surfaceFormats;
}

VkSurfaceFormatKHR PhysicalDevice::getSurfaceFormat(
    VkSurfaceKHR surface, 
    VkFormat format,
    VkColorSpaceKHR colorSpace /* = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR */
) const
{
    const std::vector<VkSurfaceFormatKHR>& formats = getSurfaceFormats(surface);

    auto it = std::find_if(formats.cbegin(), formats.cend(), [format, colorSpace] (VkSurfaceFormatKHR f) {
        return f.format == format && f.colorSpace == colorSpace;
    });

    if (it == formats.end())
    {
        return { VK_FORMAT_UNDEFINED };
    }

    return *it;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola