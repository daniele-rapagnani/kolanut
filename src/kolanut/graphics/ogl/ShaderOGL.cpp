#include "kolanut/core/Logging.h"
#include "kolanut/utils/EnumHasher.h"
#include "kolanut/graphics/ogl/ShaderOGL.h"

#include <unordered_map>

namespace kola {
namespace graphics {

namespace {

std::unordered_map<Shader::Type, utils::ogl::Shader::Type, utils::enum_hasher<Shader::Type>>
SHADER_TYPES = {
    { Shader::Type::NONE, utils::ogl::Shader::Type::NONE },
    { Shader::Type::FRAGMENT, utils::ogl::Shader::Type::FRAGMENT },
    { Shader::Type::VERTEX, utils::ogl::Shader::Type::VERTEX },
};

} // namespace

bool ShaderOGL::doLoad(const std::vector<char>& data, Type type)
{
    std::string compError;
    std::string source = std::string(data.begin(), data.end());

    assert(SHADER_TYPES.find(type) != SHADER_TYPES.end());

    this->shader = std::make_shared<utils::ogl::Shader>(
        source, 
        SHADER_TYPES.at(type)
    );

    if (!this->shader->compile(compError))
    {
        knM_logError("Can't compile shader (path: '" << getPath() << "'): " << compError);
        return false;
    }

    return true;
}

} // namespace graphics
} // namespace kola