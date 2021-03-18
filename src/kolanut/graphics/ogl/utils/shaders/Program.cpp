#include "kolanut/core/Types.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/graphics/ogl/utils/Context.h"
#include "kolanut/graphics/ogl/utils/shaders/Program.h"

#include <functional>

namespace kola {
namespace utils {
namespace ogl {

Program::~Program()
{
    if (this->programId)
    {
        knM_oglCall(glDeleteProgram(this->programId));
        this->programId = 0;
    }
}

void Program::attach(std::shared_ptr<Shader> shader)
{
    this->shaders.push_back(shader);
}

bool Program::link(std::string& error)
{
    if (this->programId == 0)
    {
        knM_oglCall(this->programId = glCreateProgram());

        if (this->programId == 0)
        {
            error.assign("Can't create program");
            return false;
        }
    }

    for (const auto& shader : this->shaders)
    {
        knM_oglCall(glAttachShader(this->programId, shader->getId()));
    }

    knM_oglCall(glLinkProgram(this->programId));

    GLint linkStatus;
    knM_oglCall(glGetProgramiv(this->programId, GL_LINK_STATUS, &linkStatus));

    if (linkStatus == GL_TRUE)
    {
        return true;
    }

    GLint infoLogSize = 0;
    knM_oglCall(glGetProgramiv(this->programId, GL_INFO_LOG_LENGTH, &infoLogSize));

    if (infoLogSize == 0)
    {
        error.assign("No info log.");
        return false;
    }

    error.resize(infoLogSize);

    knM_oglCall(glGetProgramInfoLog(this->programId, infoLogSize, nullptr, &error[0]));
    return false;
}

void Program::use() const
{
    di::get<Context>()->useProgram(this->programId);
}

GLint Program::getUniformLocation(const std::string& name) const
{
    GLint uniLoc = -1;
    knM_oglCall(uniLoc = glGetUniformLocation(this->programId, name.c_str()));
    return uniLoc;
}

GLint Program::getAttributeLocation(const std::string& name) const
{
    GLint attLoc = -1;
    knM_oglCall(attLoc = glGetAttribLocation(this->programId, name.c_str()));

    return attLoc;
}

} // namespace ogl
} // namespace utils
} // namespace kola