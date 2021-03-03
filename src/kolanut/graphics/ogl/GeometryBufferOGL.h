#pragma once

#include "kolanut/graphics/GeometryBuffer.h"

#include <glad/glad.h>

#include <vector>
#include <string>
#include <functional>
#include <cstdint>

namespace kola {
namespace graphics {

class GeometryBufferOGL : public GeometryBuffer
{
public:
    ~GeometryBufferOGL();
    
    void bind() const;
    void unbind() const;

protected:
    bool createBuffer() override;
    bool copy(const Vertex* vertices, size_t base, size_t size, size_t* realSize) override;

private:
    GLuint buffer = {};
    size_t totalSize = {};
};

} // namespace graphics
} // namespace kola