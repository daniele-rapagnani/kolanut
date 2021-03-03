#include "kolanut/graphics/ogl/ProgramOGL.h"
#include "kolanut/graphics/ogl/ShaderOGL.h"
#include "kolanut/core/Logging.h"

namespace kola {
namespace graphics {

bool ProgramOGL::link()
{
    this->program = std::make_shared<utils::ogl::Program>();

    for (const auto& pair : this->shaders)
    {
        std::shared_ptr<ShaderOGL> s = std::static_pointer_cast<ShaderOGL>(pair.second);
        this->program->attach(s->getGLShader());
    }

    std::string errors = {};

    if (!this->program->link(errors))
    {
        knM_logError("Can't link program: " << errors);
        return false;
    }

    return true;
}

} // namespace graphics
} // namespace kola