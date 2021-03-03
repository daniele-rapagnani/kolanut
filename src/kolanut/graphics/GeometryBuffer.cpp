#include "kolanut/core/Logging.h"
#include "kolanut/graphics/GeometryBuffer.h"

namespace kola {
namespace graphics {

bool GeometryBuffer::init(const Config& config /* = {} */)
{
    this->config = config;
    this->frameBufferSize = sizeof(Vertex) * getConfig().maxVerts;

    if (!createBuffer())
    {
        knM_logFatal("Can't create vertex buffer (maybe it's too large?).");
        return false;
    }

    this->bufferBase.resize(getConfig().maxFrames);
    
    for (uint8_t i = 0; i < getConfig().maxFrames; i++)
    {
        reset(i);
    }

    return true;
}

GeometryBuffer::Handle GeometryBuffer::addVertices(const Vertex* vertices, size_t count, uint8_t frame)
{
    frame = frame % getConfig().maxFrames;

    size_t size = count * sizeof(Vertex);
    size_t base = this->bufferBase[static_cast<size_t>(frame)];

    size_t realSize;

    if (!copy(vertices, base, size, &realSize))
    {
        return GeometryBuffer::NULL_HANDLE;
    }

    this->bufferBase[frame] += realSize;

    return (base << 32) | ((realSize & 0xFFFFFFF) << 4) | ((frame + 1) & 0xF);
}

} // namespace graphics
} // namespace kola