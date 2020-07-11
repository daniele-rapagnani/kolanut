#pragma once

#include "kolanut/graphics/Texture.h"

#include <SDL.h>
#include <string>

namespace kola {
namespace graphics {

class RendererSDL;

class TextureSDL : public Texture
{
public:
    friend class RendererSDL;

    TextureSDL() = default;
    ~TextureSDL();

public:
    bool load(const std::string& file) override;
    Sizei getSize() override;

    bool loadTexture(const RendererSDL& renderer);

private:
    SDL_Surface* surface = nullptr;
    SDL_Texture* texture = nullptr;
    unsigned char* data = nullptr;
};

} // namespace graphics
} // namespace kola