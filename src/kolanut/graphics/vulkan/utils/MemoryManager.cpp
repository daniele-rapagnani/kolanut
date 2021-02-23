#include "kolanut/graphics/vulkan/utils/MemoryManager.h"
#include "kolanut/graphics/vulkan/utils/Device.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"

#include <cassert>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace kola {
namespace graphics {
namespace vulkan {

void MemoryManager::Allocation::free() const
{
    this->pool->memoryMgr->free(this);
}

bool MemoryManager::init(std::shared_ptr<Device> device, const Config& config /* = {} */)
{
    this->config = config;
    this->device = device;

    VkPhysicalDeviceMemoryProperties pdmp;

    vkGetPhysicalDeviceMemoryProperties(
        getDevice()->getPhysicalDevice()->getVkHandle(),
        &pdmp
    );

    for (uint32_t i = 0; i < pdmp.memoryHeapCount; i++)
    {
        this->heaps.push_back(pdmp.memoryHeaps[i]);
        this->totalSize += this->heaps.back().size;
    }

    for (uint32_t i = 0; i < pdmp.memoryTypeCount; i++)
    {
        this->types.push_back(pdmp.memoryTypes[i]);
    }

    return true;
}

const MemoryManager::Allocation* MemoryManager::allocate(
    VkMemoryRequirements requirements,
    VkMemoryPropertyFlags flags
)
{
    auto allocateFromPool = [this, flags, requirements] (std::shared_ptr<MemoryPool> pool) -> const Allocation* {
        auto it = pool->findBlock(requirements.size);

        if (it == pool->freeList.end())
        {
            return nullptr;
        }
            
        this->allocations.emplace_back();
        Allocation& a = this->allocations.back();

        assert(pool->alignment = requirements.alignment);

        VkDeviceSize reminder = requirements.size % requirements.alignment;
        VkDeviceSize realSize = 
            reminder == 0 ? 
                requirements.size
                : requirements.size + requirements.alignment - reminder
        ;

        a.flags = flags;
        a.pool = pool;
        a.ptr = pool->memory;
        a.size = realSize;
        a.offset = it->offset;

        it->size -= realSize;
        it->offset += realSize;

        if (it->size == 0)
        {
            pool->freeList.erase(it);

            if (pool->freeList.empty())
            {
                auto it = std::find(this->elegiblePools.begin(), this->elegiblePools.end(), pool);
                assert(it != this->elegiblePools.end());
                this->elegiblePools.erase(it);
            }
        }

        knM_logDebug("Allocated " << a.size << " bytes at " << a.ptr << " offset " << a.offset  << " with alignment " << pool->alignment << " from pool: " << pool.get());

        return &this->allocations.back();
    };

    std::shared_ptr<MemoryPool> pool = findPool(this->elegiblePools, requirements, flags);

    if (pool)
    {
        const Allocation* a = allocateFromPool(pool);
        
        if (a)
        {
            return a;
        }

        pool = nullptr;
    }

    if (!pool)
    {
        knM_logDebug("Can't find memory pool for alignment " << requirements.alignment << " and type " << requirements.memoryTypeBits << ", creating one.");

        pool = allocatePool(getConfig().poolSize, requirements, flags);

        knM_logDebug("Created pool: " << pool.get());

        if (!pool)
        {
            knM_logFatal(
                "Can't allocate pool of size = " << requirements.size 
                << ", type = " << requirements.memoryTypeBits
                << ", flags = " << flags
            );

            return nullptr;
        }
    }

    assert(pool);
    return allocateFromPool(pool);
}

void MemoryManager::free(const Allocation* a)
{
    knM_logWarning("FREE NOT IMPLEMENTED");
}

bool MemoryManager::findMemoryType(
    uint32_t typeBits, 
    VkMemoryPropertyFlags flags,
    uint32_t* idx /* = nullptr */
)
{
    for (size_t i = 0; i < this->types.size(); i++)
    {
        if (typeBits > 0)
        {
            const uint32_t memoryTypeBits = (1 << i);
            const bool isRequiredMemoryType = typeBits & memoryTypeBits;

            if (!isRequiredMemoryType)
            {
                continue;
            }
        }

        if (testFlag(this->types[i].propertyFlags, flags))
        {
            if (idx)
            {
                *idx = i;
            }

            return true;
        }
    }

    return false;
}

std::shared_ptr<MemoryManager::MemoryPool> 
MemoryManager::allocatePool(size_t size, VkMemoryRequirements req, VkMemoryPropertyFlags flags)
{
    uint32_t typeIdx = 0;

    if (!findMemoryType(req.memoryTypeBits, flags, &typeIdx))
    {
        return nullptr;
    }

    std::shared_ptr<MemoryPool> pool = std::make_shared<MemoryPool>();

    VkMemoryAllocateInfo mai = {};
    mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mai.memoryTypeIndex = typeIdx;
    mai.allocationSize = size;

    if (
        vkAllocateMemory(
            getDevice()->getVkHandle(),
            &mai,
            nullptr,
            &pool->memory
        ) != VK_SUCCESS
    )
    {
        return nullptr;
    }

    pool->memoryMgr = shared_from_this();
    pool->type = this->types[typeIdx];
    pool->typeIdx = typeIdx;
    pool->heap = this->heaps[pool->type.heapIndex];
    pool->alignment = req.alignment;

    pool->freeList.emplace_back();
    pool->freeList.back().size = size;
    pool->freeList.back().offset = 0;

    this->pools.push_back(pool);
    this->elegiblePools.push_back(pool);

    return pool;
}

std::shared_ptr<MemoryManager::MemoryPool> 
MemoryManager::findPool(
    const std::vector<std::shared_ptr<MemoryPool>>& pools,
    VkMemoryRequirements req, 
    VkMemoryPropertyFlags flags
) const
{
    for (auto& p : pools)
    {
        if (!testFlag(req.memoryTypeBits, (1 << p->typeIdx)))
        {
            continue;
        }
        
        if (testFlag(p->type.propertyFlags, flags))
        {
            return p;
        }
    }

    return nullptr;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola