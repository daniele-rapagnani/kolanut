#pragma once

#include "kolanut/graphics/ogl/OpenGL.h"

#include "kolanut/graphics/glfw/RendererGLFW.h"
#include "kolanut/graphics/ogl/ProgramOGL.h"
#include "kolanut/graphics/ogl/GeometryBufferOGL.h"

#include <memory>

namespace kola {
namespace graphics {

class RendererOGL : public RendererGLFW
{ 
public:
    static constexpr const char* POS_ATTR_NAME = "a_position";
    static constexpr const char* TC_ATTR_NAME = "a_texCoord";
    static constexpr const char* COLOR_ATTR_NAME = "a_color";

    static constexpr GLuint POS_ATTR_LOC = 0;
    static constexpr GLuint TC_ATTR_LOC = 1;
    static constexpr GLuint COLOR_ATTR_LOC = 2;
    
public:
    virtual ~RendererOGL();

public:
    bool init(const Config& config) override;
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
    void bindProgramAttributesLocations(std::shared_ptr<Program> program) const;

private:
    GLuint perfQuery = {};
};

} // namespace graphics
} // namespace kola