#pragma once

#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"
#include "kolanut/graphics/vulkan/utils/Buffer.h"
#include "kolanut/graphics/Vertex.h"
#include "kolanut/graphics/vulkan/utils/CommandBuffer.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <functional>
#include <cstdint>

namespace kola {
namespace graphics {
namespace vulkan {

class UniformsBuffer : public DeviceDependent
{
public:
    using Handle = uint64_t;

public:
    static const size_t MAX_VEC4_PER_SET_DEFAULT = 48;
    static const size_t MAX_SETS = 1024;
    static const Handle NULL_HANDLE = 0;

public:
    struct Config
    {
        Config() {}

        size_t maxVec4PerSet = MAX_VEC4_PER_SET_DEFAULT;
        size_t maxSets = MAX_SETS;
        uint8_t maxFrames = 1;
    };

public:
    bool init(std::shared_ptr<Device> device, const Config& config = {});

    const Config& getConfig() const
    { return this->config; }

    Handle addUniform(const void* data, size_t bytes, uint8_t frame);

    bool bind(Handle h, std::shared_ptr<DescriptorSet> descriptorSet, uint32_t binding = 0);

    void reset(uint8_t frame)
    {
        this->bufferBase[frame] = this->frameBufferSize * frame;
    }

private:
    Config config = {};
    std::shared_ptr<Buffer> buffer = {};
    std::vector<VkDeviceSize> bufferBase = {};
    VkDeviceSize frameBufferSize = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola