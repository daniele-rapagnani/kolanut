#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/graphics/ogl/TextureOGL.h"

#include <glad/glad.h>

#include <memory>

#ifndef NDEBUG
#define knM_oglCall(x) \
    do { \
        x; \
        { \
            GLenum e = glGetError(); \
            if (e != GL_NO_ERROR) \
                knM_logError("OpenGL call error: " << __FILE__ << ":" << __LINE__ << " - " << e); \
        } \
    } while(false)
#else
#define knM_oglCall(x) x;
#endif

namespace kola {
namespace utils {
namespace ogl {

class Program;

struct DrawRequest
{
    std::shared_ptr<kola::graphics::TextureOGL> texture = nullptr;
    std::shared_ptr<utils::ogl::Program> program = nullptr;
    Transform2D transform = {};
    Rectf textureRect = {};
};

struct QuadDrawRequest : DrawRequest
{ };

void draw(QuadDrawRequest dr);

} // namespace ogl
} // namespace utils
} // namespace kola