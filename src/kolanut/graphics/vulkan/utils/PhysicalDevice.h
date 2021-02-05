
#pragma once

#include "kolanut/graphics/vulkan/utils/Instance.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"
#include "kolanut/graphics/vulkan/utils/QueueFamily.h"
#include "kolanut/graphics/vulkan/utils/VkHandle.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>
#include <cstdint>

namespace kola {
namespace graphics {
namespace vulkan {

class PhysicalDevice : public VkHandle<VkPhysicalDevice>
{
private:
    static std::vector<std::shared_ptr<PhysicalDevice>> devices;
    
public:
    static const std::vector<std::shared_ptr<PhysicalDevice>>& 
        getDevices(std::shared_ptr<Instance> instance)
    ;

public:
    PhysicalDevice(VkPhysicalDevice);

public:
    std::string getName() const;

    VkPhysicalDeviceFeatures getFeatures() const
    { return this->features; }

    VkPhysicalDeviceProperties getProprties() const
    { return this->properties; }

    VkFormatProperties getFormatProperties(VkFormat format) const;

    bool isFormatSupported(VkFormat format) const
    { return getFormatProperties(format).bufferFeatures != 0; }

    bool hasSwapchainSupport() const
    { return this->isExtensionSupported("VK_KHR_swapchain"); }

    QueueFamily getQueueFamily(QueueFamilyType type) const;
    QueueFamily getPresentationQueueFamily(VkSurfaceKHR surface) const;

    VkSurfaceCapabilitiesKHR getSurfaceCapabilities(VkSurfaceKHR surface) const;
    const std::vector<VkSurfaceFormatKHR>& getSurfaceFormats(VkSurfaceKHR surface) const;
    
    VkSurfaceFormatKHR getSurfaceFormat(
        VkSurfaceKHR surface, 
        VkFormat format,
        VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    ) const;

    bool hasDoubleBuffering(VkSurfaceKHR surface) const
    {
        auto sc = getSurfaceCapabilities(surface);
        return sc.maxImageCount >= 2;
    }

    const std::vector<VkExtensionProperties>& getSupportedExtensions() const;
    bool isExtensionSupported(const std::string& extension) const;

private:
    mutable std::vector<VkExtensionProperties> supportedExtensions = {};
    mutable std::vector<VkSurfaceFormatKHR> surfaceFormats = {};

    VkPhysicalDeviceFeatures features = {};
    VkPhysicalDeviceProperties properties = {};
    std::vector<VkQueueFamilyProperties> queueFamilies = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola