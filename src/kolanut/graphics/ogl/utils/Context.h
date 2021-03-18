#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/graphics/ogl/OpenGL.h"

#include <unordered_map>

namespace kola {
namespace utils {
namespace ogl {

class Context
{
public:
    void bindTexture(GLenum target, GLuint texture);
    void bindBuffer(GLenum target, GLuint buffer);
    void useProgram(GLuint program);

private:
    GLuint texture = 0;
    GLenum textureTarget = 0;
    GLuint program = 0;

    std::unordered_map<GLenum, GLuint> bufferBindings = {};
};

} // namespace ogl
} // namespace utils
} // namespace kola