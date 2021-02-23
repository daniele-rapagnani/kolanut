#pragma once

#include "kolanut/graphics/vulkan/utils/VkHandle.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"
#include "kolanut/graphics/vulkan/utils/Semaphore.h"
#include "kolanut/graphics/vulkan/utils/Fence.h"
#include "kolanut/graphics/vulkan/utils/Queue.h"

#include <vector>
#include <cstdint>
#include <cassert>

namespace kola {
namespace graphics {
namespace vulkan {

class Device;
class RenderPass;

class Swapchain 
    : public VkHandle<VkSwapchainKHR>
    , public DeviceDependent
{
private:
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

    inline size_t getImageDependentOffset(uint32_t index, uint8_t frame) const
    { 
        return frame * getImageViews().size() + index;
    }

    template <typename T, typename K = T>
    K getImageDependentResource(
        const std::vector<T>& vec, 
        uint32_t index, 
        uint8_t frame
    ) const
    {
        return ItemGetter<T,K>::returnItem(vec, getImageDependentOffset(index, frame));
    }

    bool acquireNext(
        uint32_t* next, 
        std::shared_ptr<Semaphore> imageReady
    ) const;

    bool present(
        std::shared_ptr<Queue> queue, 
        uint32_t index,
        std::shared_ptr<Semaphore> waitFor
    );

protected:
    const std::vector<VkFramebuffer>& getFramebuffers() const
    { return this->imageFramebuffers; }

private:
    Config config = {};
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    std::vector<VkImage> images = {};
    std::vector<VkImageView> imageViews = {};
    std::vector<VkFramebuffer> imageFramebuffers = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola