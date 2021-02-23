#pragma once

#include "kolanut/graphics/vulkan/utils/MemoryManager.h"
#include "kolanut/graphics/vulkan/utils/Device.h"
#include "kolanut/graphics/vulkan/utils/Buffer.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <cstdint>

namespace kola {
namespace graphics {
namespace vulkan {

template <typename T>
struct MappedMemory
{
    MappedMemory(const MemoryManager::Allocation* a, VkDeviceSize offset = 0)
    {
        if (a->mapped)
        {
            return;
        }
        
        if (
            vkMapMemory(
                a->pool->memoryMgr->getDevice()->getVkHandle(), 
                a->ptr,
                a->offset + offset,
                a->size,
                0,
                reinterpret_cast<void**>(&this->ptr)
            )
            != VK_SUCCESS
        )
        {
            this->ptr = nullptr;
        }

        this->allocation = a;
        this->allocation->mapped = true;
    }

    MappedMemory(std::shared_ptr<Buffer> buffer, size_t offset = 0)
        : MappedMemory(buffer->getAllocation(), offset)
    { }
    
    ~MappedMemory()
    {
        if (this->ptr && this->allocation)
        {
            vkUnmapMemory(
                this->allocation->pool->memoryMgr->getDevice()->getVkHandle(),
                this->allocation->ptr
            );

            this->ptr = nullptr;
            this->allocation->mapped = false;
            this->allocation = nullptr;
        }
    }

    T* operator*() const
    { return this->ptr; }

private:
    T* ptr = {};
    const MemoryManager::Allocation* allocation = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola