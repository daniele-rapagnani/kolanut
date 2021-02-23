#include "kolanut/graphics/vulkan/utils/Swapchain.h"
#include "kolanut/graphics/vulkan/utils/PhysicalDevice.h"
#include "kolanut/graphics/vulkan/utils/Device.h"
#include "kolanut/graphics/vulkan/utils/RenderPass.h"

#include <limits>

namespace kola {
namespace graphics {
namespace vulkan {

Swapchain::~Swapchain()
{
    if (!getFramebuffers().empty())
    {
        for (VkFramebuffer fb : getFramebuffers())
        {
            vkDestroyFramebuffer(this->device->getVkHandle(), fb, nullptr);
        }

        this->imageFramebuffers.clear();
    }

    if (*this)
    {
        vkDestroySwapchainKHR(this->device->getVkHandle(), getVkHandle(), nullptr);
        this->handle = VK_NULL_HANDLE;
    }
}

bool Swapchain::init(
    VkSurfaceKHR surface, 
    std::shared_ptr<Device> device, 
    const Config& config
)
{
    VkSurfaceCapabilitiesKHR sc = device->getPhysicalDevice()->getSurfaceCapabilities(surface);

    VkSurfaceFormatKHR surfaceFormat = device->getPhysicalDevice()->getSurfaceFormat(
        surface,
        config.format,
        config.colorSpace
    );

    if (surfaceFormat.format == VK_FORMAT_UNDEFINED)
    {
        knM_logFatal(
            "Requested surface format not supported (" << config.format << ")"
            << " on " << device->getPhysicalDevice()->getName()
        );

        return false;
    }

    if (sc.minImageCount > config.minImages || sc.maxImageCount < config.minImages)
    {
        knM_logFatal(
            "Unsupported number of min swapchain images for: " 
            << device->getPhysicalDevice()->getName()
        );

        return false;
    }

    QueueFamily presFamily = device->getPhysicalDevice()->getPresentationQueueFamily(surface);
    QueueFamily graphFamily = device->getPhysicalDevice()->getQueueFamily(QueueFamilyType::GRAPHICS);

    std::vector<uint32_t> queueFamilies;
    queueFamilies.push_back(presFamily);

    if (presFamily != graphFamily)
    {
        queueFamilies.push_back(graphFamily);
    }

    VkSwapchainCreateInfoKHR sci = {};
    sci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    sci.surface = surface;
    sci.minImageCount = config.minImages;
    sci.imageFormat = surfaceFormat.format;
    sci.imageColorSpace = surfaceFormat.colorSpace;
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

    if (vkCreateSwapchainKHR(device->getVkHandle(), &sci, nullptr, &this->handle) != VK_SUCCESS)
    {
        knM_logFatal("Can't create swapchain on: " << device->getPhysicalDevice()->getName());
        return false;
    }

    this->images = vulkan::getVulkanListFatal<VkImage>(
        std::bind(
            vkGetSwapchainImagesKHR,
            device->getVkHandle(),
            getVkHandle(),
            std::placeholders::_1,
            std::placeholders::_2
        ),
        "Can't get swapchain's image list"
    );

    this->imageViews.clear();

    for (VkImage i : this->images)
    {
        VkImageViewCreateInfo ivci = {};
        ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ivci.image = i;
        ivci.format = sci.imageFormat;
        ivci.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ivci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ivci.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        ivci.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        this->imageViews.emplace_back();

        if (
            vkCreateImageView(
                device->getVkHandle(), 
                &ivci, 
                nullptr, 
                &this->imageViews.back()
            ) != VK_SUCCESS
        )
        {
            knM_logFatal("Can't create image view for swapchain's image");
            return false;
        }
    }

    this->device = device;
    this->config = config;
    this->surface = surface;

    return true;
}

bool Swapchain::initFramebuffers(std::shared_ptr<RenderPass> renderPass)
{
    assert(renderPass);

    VkSurfaceCapabilitiesKHR sc = 
        this->getDevice()->getPhysicalDevice()->getSurfaceCapabilities(getSurface())
    ;

    for (uint8_t i = 0; i < getDevice()->getConfig().framesInFlight; i++)
    {
        for (auto it = getImageViews().cbegin(); it != getImageViews().cend(); ++it)
        {
            VkFramebufferCreateInfo fbci = {};
            fbci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            fbci.attachmentCount = 1;
            fbci.pAttachments = &(*it);
            fbci.renderPass = renderPass->getVkHandle();
            fbci.layers = 1;
            fbci.width = sc.currentExtent.width;
            fbci.height = sc.currentExtent.height;

            this->imageFramebuffers.emplace_back();

            if (
                vkCreateFramebuffer(this->device->getVkHandle(), &fbci, nullptr, &this->imageFramebuffers.back())
                != VK_SUCCESS
            )
            {
                knM_logFatal("Can't create framebuffer object");
                return false;
            }
        }
    }

    return true;
}

VkFramebuffer Swapchain::getFramebuffer(uint32_t index, uint8_t frame /* = 0 */) const
{
    return getImageDependentResource<VkFramebuffer>(
        this->imageFramebuffers, 
        index, 
        frame
    );
}

bool Swapchain::acquireNext(
    uint32_t* next, 
    std::shared_ptr<Semaphore> imageReady
) const
{
    assert(next);

    return vkAcquireNextImageKHR(
        getDevice()->getVkHandle(),
        getVkHandle(), 
        std::numeric_limits<uint64_t>::max(),
        imageReady->getVkHandle(),
        VK_NULL_HANDLE,
        next
    ) == VK_SUCCESS;
}

bool Swapchain::present(
    std::shared_ptr<Queue> queue, 
    uint32_t index, 
    std::shared_ptr<Semaphore> waitFor
)
{
    VkSwapchainKHR swapchain = getVkHandle();
    VkSemaphore wait = waitFor ? waitFor->getVkHandle() : VK_NULL_HANDLE;

    VkPresentInfoKHR pi = {};
    pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    pi.swapchainCount = 1;
    pi.pSwapchains = &this->handle;
    pi.pImageIndices = &index;
    pi.waitSemaphoreCount = wait == VK_NULL_HANDLE ? 0 : 1;
    pi.pWaitSemaphores = &wait;

    return vkQueuePresentKHR(queue->getVkHandle(), &pi) == VK_SUCCESS;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola