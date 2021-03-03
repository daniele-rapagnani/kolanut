#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/utils/EnumHasher.h"
#include "kolanut/graphics/Shader.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace kola {
namespace graphics {

class Program
{
private:
    using ShadersMap = std::unordered_map<
        Shader::Type, 
        std::shared_ptr<Shader>,
        utils::enum_hasher<Shader::Type>
    >;

public:
    void addShader(std::shared_ptr<Shader> shader);
    std::shared_ptr<Shader> getShader(Shader::Type type) const;

    virtual bool link() = 0;

protected:
    ShadersMap shaders = {};
};

} // namespace graphics
} // namespace kola