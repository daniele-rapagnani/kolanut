#pragma once

#include "kolanut/graphics/Shader.h"
#include "kolanut/graphics/ogl/utils/shaders/Shader.h"

#include <string>
#include <vector>
#include <memory>

namespace kola {
namespace graphics {

class ShaderOGL : public Shader
{
public:
    std::shared_ptr<utils::ogl::Shader> getGLShader() const
    { return this->shader; }

protected:
    bool doLoad(const std::vector<char>& data, Type type) override;

private:
    std::shared_ptr<utils::ogl::Shader> shader = {};
};

} // namespace graphics
} // namespace kola