#pragma once

#include "kolanut/graphics/Vertex.h"

#include <vector>
#include <string>
#include <functional>
#include <cstdint>

namespace kola {
namespace graphics {

class GeometryBuffer
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
    bool init(const Config& config = {});

    const Config& getConfig() const
    { return this->config; }

    Handle addVertices(const Vertex* vertices, size_t count, uint8_t frame);

    Handle addVertices(const std::vector<Vertex> vertices, uint8_t frame)
    {
        return addVertices(vertices.data(), vertices.size(), frame);
    }

    void reset(uint8_t frame)
    {
        this->bufferBase[frame] = this->frameBufferSize * frame;
    }

    size_t getBase(Handle h) const
    {
#if UINTPTR_MAX == UINT64_MAX
        return (h >> 4) & 0xFFFFFFFFFFFFFFF; 
#elif UINTPTR_MAX == UINT32_MAX
        return (h >> 4) & 0xFFFFFFF;
#else
        #error Unsupported architecture
#endif
    }

protected:
    virtual bool createBuffer() = 0;
    virtual bool copy(const Vertex* vertices, size_t base, size_t size, size_t* realSize) = 0;

    size_t getFrameBufferSize() const
    { return this->frameBufferSize; }

private:
    Config config = {};
    std::vector<size_t> bufferBase = {};
    size_t frameBufferSize = {};
};

} // namespace graphics
} // namespace kola