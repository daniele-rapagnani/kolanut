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

    SDL_Renderer* getRenderer() const
    { return this->renderer; }

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    Vec2f cameraPos = {};
    float cameraZoom = 1.0f;
};

} // namespace graphics
} // namespace kola