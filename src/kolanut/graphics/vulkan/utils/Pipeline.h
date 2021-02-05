#pragma once

#include "kolanut/graphics/vulkan/utils/VkHandle.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"

#include <vulkan/vulkan.h>

namespace kola {
namespace graphics {
namespace vulkan {

class Device;

class Pipeline : public VkHandle<VkPipeline>, public DeviceDependent
{
public:
    virtual ~Pipeline();
};

} // namespace vulkan
} // namespace graphics
} // namespace kola