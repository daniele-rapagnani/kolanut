#pragma once

#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <deque>
#include <algorithm>

namespace kola {
namespace graphics {
namespace vulkan {

class MemoryManager 
    : public DeviceDependent
    , public std::enable_shared_from_this<MemoryManager>
{
public:
    struct Config
    {
        Config() {};

        size_t poolSize = 1024 * 1024 * 128;
    };

    struct Heap : public VkMemoryHeap
    {
        Heap() = default;
        
        Heap(const VkMemoryHeap& h)
        {
            this->flags = h.flags;
            this->size = h.size;
        }

        bool isDeviceLocal() const
        { return testFlag(this->flags, VK_MEMORY_HEAP_DEVICE_LOCAL_BIT); }

        bool isMultiInstance() const
        { return testFlag(this->flags, VK_MEMORY_HEAP_MULTI_INSTANCE_BIT); }
    };

    struct MemoryType : public VkMemoryType
    {
        MemoryType() = default;

        MemoryType(const VkMemoryType& t)
        {
            this->heapIndex = t.heapIndex;
            this->propertyFlags = t.propertyFlags;
        }

        bool isDeviceLocal() const
        { return testFlag(this->propertyFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); }

        bool isHostVisible() const
        { return testFlag(this->propertyFlags, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT); }

        bool isHostCoherent() const
        { return testFlag(this->propertyFlags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); }

        bool isHostCached() const
        { return testFlag(this->propertyFlags, VK_MEMORY_PROPERTY_HOST_CACHED_BIT); }

        bool isLazilyAllocated() const
        { return testFlag(this->propertyFlags, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT); }
    };

    struct MemoryBlock
    {
        size_t size = {};
        size_t offset = {};
    };

    struct MemoryPool
    {
        std::shared_ptr<MemoryManager> memoryMgr = {};

        size_t size;
        MemoryType type = {};
        uint32_t typeIdx = {};
        Heap heap = {};
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkDeviceSize alignment = 16;
        std::deque<MemoryBlock> freeList = {};

        std::deque<MemoryBlock>::iterator findBlock(size_t size)
        {
            return std::find_if(
                this->freeList.begin(), 
                this->freeList.end(), 
                [size] (const MemoryBlock& b) {
                    return b.size >= size;
                }
            );
        }
    };

    struct Allocation
    {
        std::shared_ptr<MemoryPool> pool = {};
        VkMemoryPropertyFlags flags = {};
        VkDeviceMemory ptr = {};
        size_t size = {};
        size_t offset = {};
        
        mutable bool mapped = false;

        void free() const;
    };

public:
    bool init(std::shared_ptr<Device> device, const Config& config = {});

    const Config& getConfig() const
    { return this->config; }

    const Allocation* allocate(
        VkMemoryRequirements requirements,
        VkMemoryPropertyFlags flags
    );

    void free(const Allocation* a);

protected:
    bool findMemoryType(
        uint32_t typeBits, 
        VkMemoryPropertyFlags flags, 
        uint32_t* idx = nullptr
    );

    std::shared_ptr<MemoryPool> allocatePool(
        size_t size, 
        VkMemoryRequirements req, 
        VkMemoryPropertyFlags flags
    );

    std::shared_ptr<MemoryPool> findPool(
        const std::vector<std::shared_ptr<MemoryPool>>& pools,
        VkMemoryRequirements req, 
        VkMemoryPropertyFlags flags
    ) const;

private:
    Config config = {};

    std::vector<Heap> heaps = {};
    std::vector<MemoryType> types = {};

    std::vector<std::shared_ptr<MemoryPool>> pools = {};
    std::vector<std::shared_ptr<MemoryPool>> elegiblePools = {};
    std::deque<Allocation> allocations = {};

    VkDeviceSize totalSize;
};

} // namespace vulkan
} // namespace graphics
} // namespace kola