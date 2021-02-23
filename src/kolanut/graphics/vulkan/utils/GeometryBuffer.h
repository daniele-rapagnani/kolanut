#pragma once

#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"
#include "kolanut/graphics/vulkan/utils/Buffer.h"
#include "kolanut/graphics/vulkan/utils/Vertex.h"
#include "kolanut/graphics/vulkan/utils/CommandBuffer.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <functional>
#include <cstdint>

namespace kola {
namespace graphics {
namespace vulkan {

class GeometryBuffer : public DeviceDependent
{
public:
    using Handle = uint64_t;

public:
    static const size_t MAX_VERTS_DEFAULT = 20000 * 6;
    static const Handle NULL_HANDLE = 0;

public:
    struct Config
    {
        Config() {}

        size_t maxVerts = MAX_VERTS_DEFAULT;
        uint8_t maxFrames = 1;
    };

public:
    bool init(std::shared_ptr<Device> device, const Config& config = {});

    const Config& getConfig() const
    { return this->config; }

    Handle addVertices(const Vertex* vertices, size_t count, uint8_t frame);

    Handle addVertices(const std::vector<Vertex> vertices, uint8_t frame)
    {
        return addVertices(vertices.data(), vertices.size(), frame);
    }

    bool bind(Handle h, std::shared_ptr<CommandBuffer> commandBuf);

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