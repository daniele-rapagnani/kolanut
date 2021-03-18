#include "kolanut/graphics/ogl/utils/Context.h"
#include "kolanut/graphics/ogl/utils/GenericUtils.h"
#include "kolanut/core/Logging.h"

namespace kola {
namespace utils {
namespace ogl {

void Context::bindTexture(GLenum target, GLuint texture)
{
    if (this->textureTarget == target && this->texture == texture)
    {
        return;
    }

    knM_oglCall(glBindTexture(target, texture));
    this->texture = texture;
    this->textureTarget = target;
}

void Context::useProgram(GLuint program)
{
    if (this->program == program)
    {
        return;
    }

    knM_oglCall(glUseProgram(program));
    this->program = program;
}

void Context::bindBuffer(GLenum target, GLuint buffer)
{
    if (
        this->bufferBindings.find(target) != this->bufferBindings.end()
        && this->bufferBindings[target] == buffer
    )
    {
        return;
    }

    knM_oglCall(glBindBuffer(target, buffer));
    this->bufferBindings[target] = buffer;
}

} // namespace ogl
} // namespace utils
} // namespace kola