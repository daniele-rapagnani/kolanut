#include "kolanut/graphics/vulkan/ShaderVK.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"
#include "kolanut/utils/EnumHasher.h"
#include "kolanut/core/Logging.h"

namespace kola {
namespace graphics {

namespace {
    std::unordered_map<
        Shader::Type, 
        vulkan::ShaderModule::Type, 
        utils::enum_hasher<Shader::Type>
    >
    SHADER_TYPES = {
        { Shader::Type::NONE, vulkan::ShaderModule::Type::NONE },
        { Shader::Type::FRAGMENT, vulkan::ShaderModule::Type::FRAGMENT },
        { Shader::Type::VERTEX, vulkan::ShaderModule::Type::VERTEX },
    };
}

bool ShaderVK::doLoad(const std::vector<char>& data, Type type)
{
    vulkan::ShaderModule::Config c = {};
    c.type = SHADER_TYPES.at(type);

    this->shader = std::make_shared<vulkan::ShaderModule>();

    if (!this->shader->init(c, getDevice(), getPath(), data))
    {
        if (!this->shader->getErrors().empty())
        {
            knM_logError(
                "Can't compile shader (path: '" << getPath() << "'): " 
                << this->shader->getErrors()
            );
        }

        return false;
    }

    return true;
}

} // namespace graphics
} // namespace kola