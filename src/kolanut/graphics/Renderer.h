#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/graphics/Graphics.h"
#include "kolanut/graphics/Texture.h"
#include "kolanut/graphics/Font.h"

#include <memory>

namespace kola {
namespace graphics {

class Renderer
{
public:
    virtual bool init(const Config& config);
    
    virtual bool doInit(const Config& config) = 0;
    virtual std::shared_ptr<Texture> loadTexture(const std::string& file) = 0;
    virtual std::shared_ptr<Font> loadFont(const std::string& file, size_t size) = 0;
    
    virtual void draw(
        std::shared_ptr<Texture> t, 
        const Vec2f& position, 
        float angle, 
        const Vec2f& scale,
        const Vec2f& origin
    ) = 0;

    virtual void draw(
        std::shared_ptr<Texture> t, 
        const Vec2f& position, 
        float angle, 
        const Vec2f& scale,
        const Vec2f& origin,
        const Recti& rect,
        const Colori& color
    ) = 0;

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
    ) = 0;

    virtual void draw(
        const Vec2f& a,
        const Vec2f& b,
        const Colori& color
    ) = 0;

    virtual void setCameraPosition(const Vec2f& pos) = 0;
    virtual void setCameraZoom(float zoom) = 0;
    virtual Vec2f getCameraPosition() = 0;
    virtual float getCameraZoom() = 0;
    
    virtual void clear() = 0;
    virtual void flip() = 0;

    virtual Vec2i getResolution() = 0;

    virtual Vec2i getDesignResolution()
    { return this->designResolution; }

private:
    Vec2i designResolution = {};
};

} // namespace graphics
} // namespace kola