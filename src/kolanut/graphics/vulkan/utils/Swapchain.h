#pragma once

#include "kolanut/graphics/vulkan/utils/VkHandle.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"
#include "kolanut/graphics/vulkan/utils/Semaphore.h"
#include "kolanut/graphics/vulkan/utils/Fence.h"
#include "kolanut/graphics/vulkan/utils/Queue.h"

#include <vector>
#include <cstdint>

namespace kola {
namespace graphics {
namespace vulkan {

class Device;
class RenderPass;

class Swapchain 
    : public VkHandle<VkSwapchainKHR>
    , public DeviceDependent
{
public:
    struct Config
    {
        uint32_t minImages = 2;
        VkFormat format = VK_FORMAT_B8G8R8A8_SRGB;
        VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    };

public:
    Swapchain() = default;
    ~Swapchain();

    bool init(
        VkSurfaceKHR surface, 
        std::shared_ptr<Device> device, 
        const Config& config
    );

    bool initFramebuffers(std::shared_ptr<RenderPass> renderPass);

    const Config& getConfig() const
    { return this->config; }

    VkSurfaceKHR getSurface() const
    { return this->surface; }

    const std::vector<VkImage>& getImages() const
    { return this->images; }

    const std::vector<VkImageView>& getImageViews() const
    { return this->imageViews; }

    VkFramebuffer getFramebuffer(uint32_t index, uint8_t frame = 0) const;
    std::shared_ptr<Semaphore> getImageReadySemaphore(uint8_t frame) const;
    std::shared_ptr<Semaphore> getRenderCompleteSemaphore(uint32_t index, uint8_t frame = 0) const;
    std::shared_ptr<Fence> getFrameCompletedFence(uint8_t frame) const;

    inline size_t getResourceOffset(uint32_t index, uint8_t frame) const
    { 
        return frame * getImageViews().size() + index;
    }

    bool acquireNext(uint32_t* next, uint8_t frame) const;

    bool present(
        std::shared_ptr<Queue> queue, 
        uint32_t index,
        std::shared_ptr<Semaphore> waitFor
    );

protected:
    const std::vector<VkFramebuffer>& getFramebuffers() const
    { return this->imageFramebuffers; }

    const std::vector<std::shared_ptr<Semaphore>>& getImageReadySemaphores() const
    { return this->imageReadySemaphores; }

private:
    Config config = {};
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    std::vector<VkImage> images = {};
    std::vector<VkImageView> imageViews = {};
    std::vector<VkFramebuffer> imageFramebuffers = {};

    std::vector<std::shared_ptr<Semaphore>> imageReadySemaphores = {};
    std::vector<std::shared_ptr<Semaphore>> renderCompleteSemaphores = {};
    std::vector<std::shared_ptr<Fence>> frameCompletedFence = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola