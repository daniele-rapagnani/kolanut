#pragma once

#include "kolanut/core/Types.h"

#include <string>
#include <vector>

namespace kola {
namespace graphics {

class Shader
{
public:
    enum class Type
    {
        NONE = 0,
        VERTEX,
        FRAGMENT
    };

public:
    static constexpr const char* SHADER_FRAG_EXT = ".frag";
    static constexpr const char* SHADER_VERT_EXT = ".vert";

public:
    virtual bool load(const std::string& fileName, Type type = Type::NONE);
    virtual bool load(const std::vector<char>& data, Type type);

    Type getType() const
    { return this->type; }

    const std::string& getPath() const
    { return this->path; }

    void setPath(const std::string& path)
    { this->path = path; }

    const std::vector<char>& getSource() const
    { return this->source; }

protected:
    virtual bool doLoad(const std::vector<char>& data, Type type) = 0;

private:
    Type type = {};
    std::string path = {};
    std::vector<char> source = {};
};

} // namespace graphics
} // namespace kola