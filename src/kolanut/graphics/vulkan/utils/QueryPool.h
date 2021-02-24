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

class QueryPool : 
    public VkHandle<VkQueryPool>
    , public DeviceDependent
{
public:
    struct Config
    {
        VkQueryType type;
        size_t count;
    };

public:
    bool init(std::shared_ptr<Device> device, const Config& config);

    QueryPool() = default;
    ~QueryPool();

    const Config& getConfig() const
    { return this->config; }

private:
    Config config = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola