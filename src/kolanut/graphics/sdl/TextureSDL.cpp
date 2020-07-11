#include "kolanut/graphics/sdl/TextureSDL.h"
#include "kolanut/graphics/sdl/RendererSDL.h"
#include "kolanut/core/Logging.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace kola {
namespace graphics {

TextureSDL::~TextureSDL()
{
    if (this->texture)
    {
        SDL_DestroyTexture(this->texture);
        this->texture = nullptr;
    }

    if (this->surface)
    {
        SDL_FreeSurface(this->surface);
        this->surface = nullptr;
    }

    if (this->data)
    {
        stbi_image_free(this->data);
        this->data = nullptr;
    }
}

bool TextureSDL::load(const std::string& file)
{
    int w, h, n;
    this->data = stbi_load(file.c_str(), &w, &h, &n, STBI_rgb_alpha);
    
    if (!this->data)
    {
        knM_logError("Can't load '" << file << "': " << stbi_failure_reason());
        return false;
    }

    Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    this->surface = SDL_CreateRGBSurfaceFrom(
        this->data, 
        w, h, 
        STBI_rgb_alpha * 8, 4 * w,
        rmask, gmask, bmask, amask
    );

    if (!this->surface)
    {
        knM_logError("Can't create surface from '" << file << "': " << SDL_GetError());
        return false;
    }

    return true;
}

bool TextureSDL::loadTexture(const RendererSDL& renderer)
{
    assert(this->surface);
    this->texture = SDL_CreateTextureFromSurface(renderer.getRenderer(), this->surface);

    if (!this->texture)
    {
        knM_logError("Can't create texture: " << SDL_GetError());
        return false;
    }

    return true;
}

Sizei TextureSDL::getSize()
{
    if (!this->surface)
    {
        return {};
    }

    return { this->surface->w, this->surface->h };
}

} // namespace graphics
} // namespace kola