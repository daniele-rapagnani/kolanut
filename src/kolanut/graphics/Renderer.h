#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/graphics/Graphics.h"
#include "kolanut/graphics/Texture.h"

#include <memory>

namespace kola {
namespace graphics {

class Renderer
{
public:
    virtual bool init(const Config& config) = 0;
    virtual std::shared_ptr<Texture> loadTexture(const std::string& file) = 0;
    
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
        const Vec4i& rect
    ) = 0;

    virtual void setCameraPosition(const Vec2f& pos) = 0;
    virtual void setCameraZoom(float zoom) = 0;
    virtual Vec2f getCameraPosition() = 0;
    virtual float getCameraZoom() = 0;
    
    virtual void clear() = 0;
    virtual void flip() = 0;

    virtual Vec2i getResolution() = 0;
};

} // namespace graphics
} // namespace kola