#pragma once

#include "kolanut/graphics/vulkan/utils/VkHandle.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <cstdint>
#include <vector>

namespace kola {
namespace graphics {
namespace vulkan {

class Fence : 
    public VkHandle<VkFence>
    , public DeviceDependent
{
public:
    bool init(std::shared_ptr<Device> device, bool signaled = false);
    bool reset() const;

    bool wait(uint64_t timeout = 0) const;

    Fence() = default;
    ~Fence();
};

} // namespace vulkan
} // namespace graphics
} // namespace kola