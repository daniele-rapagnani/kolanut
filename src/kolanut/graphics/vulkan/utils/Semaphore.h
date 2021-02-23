#pragma once

#include "kolanut/graphics/vulkan/utils/VkHandle.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace kola {
namespace graphics {
namespace vulkan {

class Semaphore : 
    public VkHandle<VkSemaphore>, 
    public DeviceDependent
{
public:
    bool init(std::shared_ptr<Device> device);

    Semaphore() = default;
    ~Semaphore();
};

} // namespace vulkan
} // namespace graphics
} // namespace kola