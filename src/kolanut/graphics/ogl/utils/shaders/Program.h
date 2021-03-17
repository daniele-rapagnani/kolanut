#pragma once

#include "kolanut/core/Logging.h"
#include "kolanut/graphics/ogl/utils/shaders/Shader.h"
#include "kolanut/graphics/ogl/utils/GenericUtils.h"

#include "kolanut/graphics/ogl/OpenGL.h"
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <memory>

namespace kola {
namespace utils {
namespace ogl {

namespace _internal {

    template<typename T> 
    struct UniformSetter
    {
        static bool set(GLint loc, const T& val) = delete;
    };

    template<>
    struct UniformSetter<Vec2f>
    {
        static bool set(GLint loc, const Vec2f& val)
        {
            knM_oglCall(glUniform2f(loc, val.x, val.y));
            return true;
        }
    };

    template<>
    struct UniformSetter<Vec2i>
    {
        static bool set(GLint loc, const Vec2i& val)
        {
            knM_oglCall(glUniform2i(loc, val.x, val.y));
            return true;
        }
    };

    template<>
    struct UniformSetter<Mat3f>
    {
        static bool set(GLint loc, const Mat3f& val)
        {
            knM_oglCall(glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(val)));
            return true;
        }
    };

    template<>
    struct UniformSetter<Mat4f>
    {
        static bool set(GLint loc, const Mat4f& val)
        {
            knM_oglCall(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val)));
            return true;
        }
    };

} // namespace _internal

class Program
{
public:
    ~Program();

    void attach(std::shared_ptr<Shader> shader);
    bool link(std::string& error);

    template <typename T>
    bool setUnifrom(const std::string& name, const T& val) const
    {
        GLint loc = getUniformLocation(name);

        if (loc < 0)
        {
            return false;
        }

        return _internal::UniformSetter<T>::set(loc, val);
    }

    GLint getAttributeLocation(const std::string& name) const;

    void use() const;

    GLuint getId() const
    { return this->programId; }

private:
    GLint getUniformLocation(const std::string& name) const;

private:
    GLuint programId = {};
    std::vector<std::shared_ptr<Shader>> shaders = {};
};

} // namespace ogl
} // namespace utils
} // namespace kola