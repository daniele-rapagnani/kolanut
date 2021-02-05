#pragma once

#include <vulkan/vulkan.h>

namespace kola {
namespace graphics {
namespace vulkan {

template <typename T>
class VkHandle
{
public:
    VkHandle() = default;
    
    VkHandle(T handle)
        : handle(handle)
    { }
    
    T getVkHandle() const
    { return this->handle; }

    operator bool() { return this->handle != VK_NULL_HANDLE; }
    bool operator!() const { return this->handle == VK_NULL_HANDLE; }

protected:
    T handle = VK_NULL_HANDLE;
};

} // namespace vulkan
} // namespace graphics
} // namespace kola