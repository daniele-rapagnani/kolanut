#include "kolanut/graphics/Program.h"

#include <cassert>

namespace kola {
namespace graphics {

void Program::addShader(std::shared_ptr<Shader> shader)
{
    assert(shader->getType() != Shader::Type::NONE);

    if (shader->getType() == Shader::Type::NONE)
    {
        return;
    }

    this->shaders[shader->getType()] = shader;
}

std::shared_ptr<Shader> Program::getShader(Shader::Type type) const
{
    if (this->shaders.find(type) == this->shaders.end())
    {
        return nullptr;
    }

    return this->shaders.at(type);
}

} // namespace graphics
} // namespace kola