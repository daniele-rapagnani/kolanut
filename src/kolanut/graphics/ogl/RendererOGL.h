#pragma once

#include <glad/glad.h>

#include "kolanut/graphics/glfw/RendererGLFW.h"
#include "kolanut/graphics/ogl/ProgramOGL.h"
#include "kolanut/graphics/ogl/GeometryBufferOGL.h"

#include <memory>

namespace kola {
namespace graphics {

class RendererOGL : public RendererGLFW
{ 
public:
    virtual ~RendererOGL();

public:
    bool doInit(const Config& config) override;

    std::shared_ptr<Program> createProgram(DrawMode mode) override;

    void drawSurface(const DrawSurface& req) override;

    void doClear() override;
    void doFlip() override;

    void onUpdateWindowSize() override;

protected:
    std::shared_ptr<Texture> createTexture() override;
    std::shared_ptr<Font> createFont() override;
    std::shared_ptr<Shader> createShader() override;
    std::shared_ptr<GeometryBuffer> createGeometryBuffer() override;

    std::string getShadersExt() const override
    { return ".ogl"; }

private:
    GLuint perfQuery = {};
    std::shared_ptr<Texture> lastTexture = {};
};

} // namespace graphics
} // namespace kola