#include "kolanut/graphics/vulkan/utils/Texture.h"
#include "kolanut/graphics/vulkan/utils/Device.h"

#include <limits>

namespace kola {
namespace graphics {
namespace vulkan {

bool Texture::init(
    std::shared_ptr<Device> device, 
    uint8_t* data,
    size_t width,
    size_t height
)
{
    this->device = device;
    this->width = width;
    this->height = height;

    size_t size = getWidth() * getHeight() * 4;

    knM_logDebug("Creating texture " << width << "x" << height << " staging buffer.");

    this->stagingBuffer = make_init<Buffer>(
        this->device, 
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT
    );

    if (!this->stagingBuffer)
    {
        return false;
    }

    if (!this->stagingBuffer->copy(data, size, 0))
    {
        return false;
    }

    VkImageCreateInfo tici = {};
    tici.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    tici.format = VK_FORMAT_R8G8B8A8_SRGB;
    tici.imageType = VK_IMAGE_TYPE_2D;
    tici.mipLevels = 1;
    tici.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    tici.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    tici.tiling = VK_IMAGE_TILING_OPTIMAL;
    tici.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    tici.samples = VK_SAMPLE_COUNT_1_BIT;
    tici.arrayLayers = 1;
    tici.extent.width = getWidth();
    tici.extent.height = getHeight();
    tici.extent.depth = 1;

    if (vkCreateImage(this->device->getVkHandle(), &tici, nullptr, &this->image) != VK_SUCCESS)
    {
        return false;
    }

    VkMemoryRequirements req = {};
    vkGetImageMemoryRequirements(this->device->getVkHandle(), this->image, &req);

    knM_logDebug("Allocating texture " << width << "x" << height << " memory.");

    this->textureMem = this->device->getMemoryManager()->allocate(
        req,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    if(!this->textureMem)
    {
        return false;
    }

    if (
        vkBindImageMemory(
            this->device->getVkHandle(), 
            this->image, 
            this->textureMem->ptr, 
            this->textureMem->offset
        ) != VK_SUCCESS
    )
    {
        knM_logFatal("Can't bind texture image memory");
        return false;
    }

    VkImageViewCreateInfo tvici = {};
    tvici.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    tvici.format = tici.format;
    tvici.image = this->image;
    tvici.viewType = VK_IMAGE_VIEW_TYPE_2D;
    tvici.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    tvici.subresourceRange.layerCount = 1;
    tvici.subresourceRange.levelCount = 1;

    if (vkCreateImageView(this->device->getVkHandle(), &tvici, nullptr, &this->imageView) != VK_SUCCESS)
    {
        knM_logFatal("Can't create kitten texture image view");
        return false;
    }

    return true;
}

bool Texture::transferToGPU(std::shared_ptr<Queue> queue) const
{
    VkBuffer ib = this->stagingBuffer->getVkHandle();
    VkImage ti = this->image;

    bool result = queue->submitOneShot([ib, ti, this] (VkCommandBuffer b) {
        VkImageMemoryBarrier barrier = {};

        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.image = ti;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(
            b,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        VkBufferImageCopy bic = {};
        bic.bufferOffset = 0;
        bic.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        bic.imageSubresource.layerCount = 1;
        bic.imageExtent.depth = 1;
        bic.imageExtent.width = this->getWidth();
        bic.imageExtent.height = this->getHeight();

        vkCmdCopyBufferToImage(b, ib, ti, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bic);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            b,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        return true;
    });
    
    return result;
}

void Texture::use(VkSampler sampler, std::shared_ptr<DescriptorSet> set, uint32_t binding /* = 0 */)
{
    VkDescriptorImageInfo dii = {};
    dii.sampler = sampler;
    dii.imageView = this->imageView;
    dii.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet wds = {};
    wds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    wds.dstSet = set->getVkHandle();
    wds.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    wds.dstBinding = binding;
    wds.descriptorCount = 1;
    wds.pImageInfo = &dii;

    vkUpdateDescriptorSets(this->device->getVkHandle(), 1, &wds, 0, nullptr);
}

Texture::~Texture()
{
    if (this->imageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(getDevice()->getVkHandle(), this->imageView, nullptr);
        this->imageView = VK_NULL_HANDLE;
    }

    if (this->image != VK_NULL_HANDLE)
    {
        vkDestroyImage(getDevice()->getVkHandle(), this->image, nullptr);
        this->image = VK_NULL_HANDLE;
    }

    if (this->textureMem)
    {
        this->textureMem->free();
        this->textureMem = nullptr;
    }
}

} // namespace vulkan
} // namespace graphics
} // namespace kola