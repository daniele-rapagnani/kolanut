#pragma once

#include "kolanut/graphics/Program.h"
#include "kolanut/graphics/ogl/utils/shaders/Program.h"

#include <string>

namespace kola {
namespace graphics {

class ProgramOGL : public Program
{
public:
    virtual bool link() override;

    std::shared_ptr<utils::ogl::Program> getGLProgram() const
    { return this->program; }

private:
    std::shared_ptr<utils::ogl::Program> program = {};
};

} // namespace graphics
} // namespace kola