#include "kolanut/graphics/Shader.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/filesystem/FilesystemEngine.h"

namespace kola {
namespace graphics {

namespace {

bool checkExtension(const std::string& str, const std::string& ext)
{
    if (str.size() < ext.size())
    {
        return false;
    }

    for (size_t i = 0; i < ext.size(); i++)
    {
        if (str[str.size() - i - 1] != ext[ext.size() - i - 1])
        {
            return false;
        }
    }

    return true;
}

} // namespace

bool Shader::load(
    const std::string& fileName, 
    Type type /* = Type::NONE */
)
{
    this->source.clear();

    if (type == Type::NONE)
    {
        if (checkExtension(fileName, Shader::SHADER_FRAG_EXT))
        {
            type = Type::FRAGMENT;
        }
        else if (checkExtension(fileName, Shader::SHADER_VERT_EXT))
        {
            type = Type::VERTEX;
        }
        else
        {
            knM_logError("Can't guess shader extension from path: " << fileName);
            return false;
        }
    }

    if (!di::get<filesystem::FilesystemEngine>()->getFileContent(fileName, this->source))
    {
        return false;
    }

    this->path = fileName;

    return load(this->source, type);
}

bool Shader::load(const std::vector<char>& data, Type type)
{
    this->type = type;
    
    if (!doLoad(data, type))
    {
        return false;
    }

    if (&this->source != &data)
    {
        this->source = data;
    }

    return true;
}

} // namespace graphics
} // namespace kola