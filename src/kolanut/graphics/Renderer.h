#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/graphics/Graphics.h"
#include "kolanut/graphics/Texture.h"
#include "kolanut/graphics/Font.h"
#include "kolanut/graphics/Shader.h"
#include "kolanut/graphics/Program.h"
#include "kolanut/graphics/GeometryBuffer.h"

#include <vector>
#include <memory>

namespace kola {
namespace graphics {

class Renderer
{
public:
    enum class DrawMode
    {
        NONE,
        TRIANGLES,
        LINES
    };

    struct DrawSurface
    {
        std::shared_ptr<Program> program = {};
        std::shared_ptr<Texture> texture = {};
        Transform3D transform = {};
        Transform3D camera = {};
        void* vertices = {};
        DrawMode mode = DrawMode::TRIANGLES;
        size_t verticesCount = 0;
    };

public:
    virtual bool init(const Config& config);
    
    virtual bool doInit(const Config& config) = 0;
    virtual std::shared_ptr<Texture> loadTexture(const std::string& file);
    virtual std::shared_ptr<Font> loadFont(const std::string& file, size_t size);
    virtual std::shared_ptr<Shader> loadShader(
        const std::string& file, 
        Shader::Type type = Shader::Type::NONE
    );

    virtual std::shared_ptr<Program> createProgram(DrawMode mode) = 0;

    virtual void draw(
        std::shared_ptr<Texture> t, 
        const Transform3D& tr,
        const Recti& rect,
        const Colori& color
    );

    virtual void draw(
        const char* str,
        size_t len,
        std::shared_ptr<Font> f, 
        const Vec2f& position, 
        const Vec2f& scale,
        const Colori& color
    );

    void draw(
        const std::string& str,
        std::shared_ptr<Font> f, 
        const Vec2f& position, 
        const Vec2f& scale,
        const Colori& color
    )
    {
        draw(str.c_str(), str.size(), f, position, scale, color);
    }

    virtual void draw(
        const Rectf& rect,
        const Colori& color
    );

    virtual void draw(
        const Vec2f& a,
        const Vec2f& b,
        const Colori& color
    );

    virtual void drawSurface(const DrawSurface& req) = 0;

    virtual void setCameraPosition(const Vec2f& pos);
    virtual void setCameraZoom(float zoom);
    virtual Vec2f getCameraPosition() const;
    virtual float getCameraZoom() const;
    
    void clear();
    void flip();

    virtual void doClear() = 0;
    virtual void doFlip() = 0;

    virtual Sizei getResolution() const = 0;
    virtual float getPixelsPerPoint() const;
    virtual Sizei getPixelResolution() const = 0;

    virtual Sizei getDesignResolution() const
    { return this->designResolution; }

    uint8_t getCurrentFrame() const
    { return this->currentInFlightFrame; }

    const Config& getConfig() const
    { return this->config; }

    std::shared_ptr<Program> getProgram() const
    { return this->program; }

    std::shared_ptr<Program> getLineProgram() const
    { return this->lineProgram; }

    std::shared_ptr<GeometryBuffer> getGeometryBuffer() const
    { return this->geometryBuffer; }

    void createCameraTransform(Transform3D& tr) const;

protected:
    virtual std::shared_ptr<Font> createFont() = 0;
    virtual std::shared_ptr<Shader> createShader() = 0;
    virtual std::shared_ptr<Texture> createTexture() = 0;
    virtual std::shared_ptr<GeometryBuffer> createGeometryBuffer() = 0;
    virtual std::shared_ptr<Program> createMainProgram();
    virtual std::shared_ptr<Program> createLineProgram();

    virtual std::string getShadersExt() const
    { return ""; }

protected:
    std::shared_ptr<Program> program = {};
    std::shared_ptr<Program> lineProgram = {};
    std::shared_ptr<GeometryBuffer> geometryBuffer = {};

private:
    Config config = {};
    Vec2f cameraPos = {};
    float cameraZoom = 1.0f;
    Sizei designResolution = {};
    uint8_t currentInFlightFrame = 0;

    std::vector<DrawSurface> jobs = {};
};

} // namespace graphics
} // namespace kola