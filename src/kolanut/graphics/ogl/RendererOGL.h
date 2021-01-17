#pragma once

#include "kolanut/graphics/Renderer.h"
#include "kolanut/graphics/ogl/utils/shaders/Program.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <memory>

namespace kola {
namespace graphics {

class RendererOGL : public Renderer
{ 
public:
    static constexpr const char* SHADER_FRAG_EXT = ".frag";
    static constexpr const char* SHADER_VERT_EXT = ".vert";

public:
    ~RendererOGL();

public:
    bool doInit(const Config& config) override;
    std::shared_ptr<Texture> loadTexture(const std::string& file) override;
    std::shared_ptr<Font> loadFont(const std::string& file, size_t sizes) override;
    
    void draw(
        std::shared_ptr<Texture> t, 
        const Vec2f& position, 
        float angle, 
        const Vec2f& scale,
        const Vec2f& origin
    ) override;

    void draw(
        std::shared_ptr<Texture> t, 
        const Vec2f& position, 
        float angle, 
        const Vec2f& scale,
        const Vec2f& origin, 
        const Vec4i& rect,
        const Colori& color
    ) override;

    void draw(
        const Rectf& rect,
        const Colori& color
    ) override;

    void draw(
        const Vec2f& a,
        const Vec2f& b,
        const Colori& color
    ) override;

    void clear() override;
    void flip() override;

    Vec2i getResolution() override;

    void setCameraPosition(const Vec2f& pos) override;
    void setCameraZoom(float zoom) override;
    Vec2f getCameraPosition() override;
    float getCameraZoom() override;

    std::shared_ptr<utils::ogl::Shader> loadShader(
        const std::string& path,
        std::string& error,
        utils::ogl::Shader::Type type = utils::ogl::Shader::Type::NONE
    );

    Vec2i getPixelResolution();
    uint8_t getPixelsPerPoint();

    GLFWwindow* getWindow() const
    { return this->window; }

    void updateWindowSize();

private:
    Vec2f cameraPos = {};
    float cameraZoom = 1.0f;

    GLFWwindow* window = nullptr;

    std::shared_ptr<utils::ogl::Program> drawProgram = nullptr;
};

} // namespace graphics
} // namespace kola