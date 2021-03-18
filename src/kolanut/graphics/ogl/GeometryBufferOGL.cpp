#include "kolanut/graphics/ogl/GeometryBufferOGL.h"
#include "kolanut/graphics/ogl/utils/Context.h"
#include "kolanut/graphics/ogl/utils/GenericUtils.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"

#include <cassert>

namespace kola {
namespace graphics {

GeometryBufferOGL::~GeometryBufferOGL()
{
    if (this->buffer)
    {
        knM_oglCall(glDeleteBuffers(1, &this->buffer));
    }
}

void GeometryBufferOGL::bind()
{
    di::get<utils::ogl::Context>()->bindBuffer(GL_ARRAY_BUFFER, this->buffer);
}

void GeometryBufferOGL::unbind()
{
    di::get<utils::ogl::Context>()->bindBuffer(GL_ARRAY_BUFFER, 0);
}

bool GeometryBufferOGL::createBuffer()
{
    knM_oglCall(glGenBuffers(1, &this->buffer));
    bind();
    
    this->totalSize = getFrameBufferSize() * getConfig().maxFrames;
    
    knM_oglCall(glBufferData(
        GL_ARRAY_BUFFER, 
        this->totalSize,
        nullptr,
        GL_STREAM_DRAW
    ));

    if (glGetError() != GL_NO_ERROR)
    {
        return false;
    }

    return true;
}

bool GeometryBufferOGL::copy(const Vertex* vertices, size_t base, size_t size, size_t* realSize)
{
    assert(realSize);
    
    if (base + size > this->totalSize)
    {
        return false;
    }

    bind();
    glBufferSubData(GL_ARRAY_BUFFER, base, size, vertices);

    unbind();
    *realSize = size;

    return true;
}

} // namespace graphics
} // namespace kola