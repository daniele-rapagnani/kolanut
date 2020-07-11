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
        Vec2f position, 
        float angle, 
        Vec2f scale
    ) = 0;

    virtual void draw(
        std::shared_ptr<Texture> t, 
        Vec2f position, 
        float angle, 
        Vec2f scale, 
        Vec4i rect
    ) = 0;
    
    virtual void clear() = 0;
    virtual void flip() = 0;
};

} // namespace graphics
} // namespace kola