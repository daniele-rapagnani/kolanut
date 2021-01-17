#include "kolanut/core/Logging.h"
#include "kolanut/graphics/ogl/utils/shaders/Shader.h"
#include "kolanut/graphics/ogl/utils/GenericUtils.h"

namespace kola {
namespace utils {
namespace ogl {

Shader::~Shader()
{
    if (this->shaderId)
    {
        knM_oglCall(glDeleteShader(this->shaderId));
        this->shaderId = 0;
    }
}

bool Shader::compile(std::string& error)
{
    if (this->source.empty())
    {
        error.assign("Missing source code");
        return false;
    }

    if (this->shaderId == 0)
    {
        knM_oglCall(
            this->shaderId = glCreateShader(
                this->type == Type::FRAGMENT ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER
            )
        );

        if (this->shaderId == 0)
        {
            error.assign("Can't create OGL shader");
            return false;
        }
    }

    const GLchar* sources[1];
    sources[0] = this->source.c_str();

    knM_oglCall(
        glShaderSource(
            this->shaderId,
            1,
            sources,
            nullptr
        )
    );

    knM_oglCall(glCompileShader(this->shaderId));

    GLint status = 0;
    knM_oglCall(glGetShaderiv(this->shaderId, GL_COMPILE_STATUS, &status));
    
    if (status == GL_TRUE)
    {
        return true;
    }

    GLint infoLogSize = 0;
    knM_oglCall(glGetShaderiv(this->shaderId, GL_INFO_LOG_LENGTH, &infoLogSize));

    if (infoLogSize == 0)
    {
        error.assign("No info log.");
        return false;
    }

    error.resize(infoLogSize);

    knM_oglCall(glGetShaderInfoLog(this->shaderId, infoLogSize, nullptr, &error[0]));
    return false;
}

} // namespace ogl
} // namespace utils
} // namespace kola