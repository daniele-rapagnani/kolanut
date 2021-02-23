#pragma once

#include "kolanut/graphics/vulkan/utils/VkHandle.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <cstdint>
#include <vector>
#include <functional>

namespace kola {
namespace graphics {
namespace vulkan {

class CommandBuffer : 
    public VkHandle<VkCommandBuffer>, 
    public DeviceDependent,
    public std::enable_shared_from_this<CommandBuffer>
{
public:
    using Runner = std::function<bool(VkCommandBuffer buffer)>;

public:
    CommandBuffer() = default;
    ~CommandBuffer();

    bool init(
        std::shared_ptr<Device> device, 
        VkCommandPool pool,
        VkCommandBufferLevel level
    );

    bool begin(VkCommandBufferUsageFlags usage = 0);
    void end();

    bool run(
        Runner runner, 
        VkCommandBufferUsageFlags usage = 0
    );

private:
    VkCommandPool pool = {};
    VkCommandBufferLevel level = {};
    bool begun = false;
};

} // namespace vulkan
} // namespace graphics
} // namespace kola