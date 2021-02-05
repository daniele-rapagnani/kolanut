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

    for (size_t frame = 0; frame < device->getConfig().framesInFlight; frame++)
    {   
        std::shared_ptr<Semaphore> s = std::make_shared<Semaphore>();

        if (!s->init(device))
        {
            knM_logFatal("Can't create image ready seamphore for swapchain image frame #" << frame);
            return false;
        }

        this->imageReadySemaphores.push_back(s);

        std::shared_ptr<Fence> f = std::make_shared<Fence>();

        if (!f->init(device, true))
        {
            knM_logFatal("Can't create frame ready fence for frame #" << frame);
            return false;
        }

        this->frameCompletedFence.push_back(f);

        for (auto it = getImageViews().cbegin(); it != getImageViews().cend(); ++it)
        {
            std::shared_ptr<Semaphore> renderComplete = std::make_shared<Semaphore>();

            if (!renderComplete->init(device))
            {
                knM_logFatal("Can't create render complete semaphore #" << frame);
                return false;
            }

            this->renderCompleteSemaphores.push_back(renderComplete);
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

namespace {

template <typename T, typename K>
struct ItemGetter
{
    static K returnItem(const std::vector<T>& vec, typename std::vector<T>::size_type i)
        = delete
    ;
};

template <typename T>
struct ItemGetter<T, T>
{
    static T returnItem(const std::vector<T>& vec, typename std::vector<T>::size_type i)
    {
        return vec[i];
    }
};

template <typename T>
struct ItemGetter<T, const T*>
{
    static const T* returnItem(const std::vector<T>& vec, typename std::vector<T>::size_type i)
    {
        return &vec[i];
    }
};

template <typename T, typename K = T>
K getOffsetedResource(
    const Swapchain* sc, 
    const std::vector<T>& vec, 
    uint32_t index, 
    uint8_t frame
)
{
    if (frame >= sc->getDevice()->getConfig().framesInFlight)
    {
        return VK_NULL_HANDLE;
    }

    if (index >= vec.size())
    {
        return VK_NULL_HANDLE;
    }

    return ItemGetter<T,K>::returnItem(vec, sc->getResourceOffset(index, frame));
}

} // namespace

VkFramebuffer Swapchain::getFramebuffer(uint32_t index, uint8_t frame /* = 0 */) const
{
    return getOffsetedResource<VkFramebuffer>(
        this, 
        this->imageFramebuffers, 
        index, 
        frame
    );
}

std::shared_ptr<Semaphore> Swapchain::getRenderCompleteSemaphore(
    uint32_t index, 
    uint8_t frame /* = 0 */
) const
{
    return getOffsetedResource<std::shared_ptr<Semaphore>>(
        this, 
        this->renderCompleteSemaphores, 
        index, 
        frame
    );
}

std::shared_ptr<Semaphore> Swapchain::getImageReadySemaphore(uint8_t frame) const
{
    if (frame >= this->imageReadySemaphores.size())
    {
        return nullptr;
    }

    return this->imageReadySemaphores[frame];
}

std::shared_ptr<Fence> Swapchain::getFrameCompletedFence(uint8_t frame) const
{
    if (frame >= this->frameCompletedFence.size())
    {
        return nullptr;
    }

    return this->frameCompletedFence[frame];
}

bool Swapchain::acquireNext(uint32_t* next, uint8_t frame) const
{
    assert(next);
    assert(frame < getDevice()->getConfig().framesInFlight);

    return vkAcquireNextImageKHR(
        getDevice()->getVkHandle(),
        getVkHandle(), 
        std::numeric_limits<uint64_t>::max(),
        getImageReadySemaphore(frame)->getVkHandle(),
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