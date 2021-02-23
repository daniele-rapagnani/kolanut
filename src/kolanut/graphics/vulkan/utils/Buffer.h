#pragma once

#include "kolanut/graphics/vulkan/utils/VkHandle.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"
#include "kolanut/graphics/vulkan/utils/MemoryManager.h"
#include "kolanut/graphics/vulkan/utils/DescriptorSet.h"
#include "kolanut/graphics/vulkan/utils/Queue.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <cstdint>
#include <vector>

namespace kola {
namespace graphics {
namespace vulkan {

class Buffer : 
    public VkHandle<VkBuffer>, 
    public DeviceDependent, 
    public std::enable_shared_from_this<Buffer>
{
public:
    bool init(
        std::shared_ptr<Device> device, 
        VkDeviceSize size, 
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags memoryProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    Buffer() = default;
    ~Buffer();

    bool copy(const void* source, VkDeviceSize size, size_t offset);

    template <typename T>
    typename std::enable_if<!std::is_pointer<T>::value, bool>::type
    copy(const T& source, size_t offset = 0)
    {
        return copy(&source, sizeof(T), offset);
    }

    template <typename T>
    bool copy(const std::vector<T>& source, size_t offset = 0)
    {
        return copy(source.data(), source.size() * sizeof(T), offset);
    }

    bool transferNow(std::shared_ptr<Buffer> dst, std::shared_ptr<Queue> queue);
    bool transfer(std::shared_ptr<Buffer> dst, std::shared_ptr<CommandBuffer> cb);

    bool bind(
        std::shared_ptr<DescriptorSet> descriptorSet, 
        uint32_t binding = 0, 
        VkDeviceSize offset = 0
    );

    const MemoryManager::Allocation* getAllocation() const
    { return this->allocation; }

private:
    VkMemoryRequirements requirements = {};
    const MemoryManager::Allocation* allocation = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola