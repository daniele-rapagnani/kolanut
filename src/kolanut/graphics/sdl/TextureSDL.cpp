#include "kolanut/graphics/sdl/TextureSDL.h"
#include "kolanut/graphics/sdl/RendererSDL.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"

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

    if (this->data && this->ownsData)
    {
        stbi_image_free(this->data);
        this->data = nullptr;
    }
}

bool TextureSDL::load(const std::string& file)
{
    std::vector<char> buffer;
    
    if (!di::get<filesystem::FilesystemEngine>()->getFileContent(file, buffer))
    {
        knM_logError("Can't read '" << file << "'");
        return false;
    }

    int w, h, n;

    unsigned char* data = stbi_load_from_memory(
        reinterpret_cast<const unsigned char*>(&buffer[0]), 
        buffer.size(), 
        &w, 
        &h, 
        &n, 
        STBI_rgb_alpha
    );
    
    if (!data)
    {
        knM_logError("Can't load '" << file << "': " << stbi_failure_reason());
        return false;
    }

    if (!load(data, w, h))
    {
        knM_logError("Can't create surface from '" << file << "': " << SDL_GetError());
        return false;
    }
    
    this->ownsData = true;
    
    return true;
}

bool TextureSDL::load(unsigned char* data, size_t w, size_t h)
{
    this->data = data;
    this->ownsData = false;

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

    return this->surface;
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