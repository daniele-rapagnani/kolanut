#pragma once

#include "kolanut/graphics/ogl/OpenGL.h"

#include <string>

namespace kola {
namespace utils {
namespace ogl {

class Shader
{
public:
    enum class Type
    {
        NONE,
        FRAGMENT,
        VERTEX
    };

public:
    Shader(const std::string& source, Type type)
        : source(source), type(type)
    {}

    ~Shader();

    bool compile(std::string& error);

    GLuint getId()
    { return this->shaderId; }

private:
    Type type = {};
    GLuint shaderId = 0;
    std::string source;
};

} // namespace ogl
} // namespace utils
} // namespace kola