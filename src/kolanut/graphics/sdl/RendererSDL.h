#pragma once

#include "kolanut/graphics/Renderer.h"

#include <SDL.h>

namespace kola {
namespace graphics {

class RendererSDL : public Renderer
{ 
public:
    ~RendererSDL();

public:
    bool init(const Config& config) override;
    std::shared_ptr<Texture> loadTexture(const std::string& file) override;
    
    void draw(
        std::shared_ptr<Texture> t, 
        Vec2f position, 
        float angle, 
        Vec2f scale,
        Vec2f origin
    ) override;

    void draw(
        std::shared_ptr<Texture> t, 
        Vec2f position, 
        float angle, 
        Vec2f scale,
        Vec2f origin, 
        Vec4i rect
    ) override;

    void clear() override;
    void flip() override;

    Vec2i getResolution() override;

    SDL_Renderer* getRenderer() const
    { return this->renderer; }

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};

} // namespace graphics
} // namespace kola