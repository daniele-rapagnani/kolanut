#include "kolanut/graphics/sdl/RendererSDL.h"
#include "kolanut/graphics/sdl/TextureSDL.h"
#include "kolanut/core/Logging.h"

#include <cassert>

namespace kola {
namespace graphics {

RendererSDL::~RendererSDL()
{
    if (this->renderer)
    {
        SDL_DestroyRenderer(this->renderer);
        this->renderer = nullptr;
    }

    if (this->window)
    {
        SDL_DestroyWindow(this->window);
        this->window = nullptr;
    }
}

bool RendererSDL::init(const Config& config)
{
    knM_logDebug("Initilizing SDL renderer");

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		knM_logFatal("SDL_Init Error: " << SDL_GetError());
	}

    Uint32 wFlags = SDL_WINDOW_SHOWN;

    if (config.resolution.fullScreen)
    {
        wFlags |= SDL_WINDOW_FULLSCREEN;
    }

	this->window = SDL_CreateWindow(
        config.windowTitle.c_str(), 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        config.resolution.screenSize.x, 
        config.resolution.screenSize.y, 
        wFlags
    );

	if (!this->window) 
	{
		knM_logFatal("Can't create SDL window: " << SDL_GetError());
		return false;
	}

    Uint32 flags = SDL_RENDERER_ACCELERATED;

    if (config.resolution.vSynced)
    {
        flags |= SDL_RENDERER_PRESENTVSYNC;
    }

	this->renderer = SDL_CreateRenderer(this->window, -1, flags);

	if (!this->renderer) 
	{
		knM_logFatal("Can't create SDL renderer: " << SDL_GetError());
		return false;
	}

    return true;
}

std::shared_ptr<Texture> RendererSDL::loadTexture(const std::string& file)
{
    knM_logDebug("Loading texture: " << file);
    
    std::shared_ptr<TextureSDL> texture = std::make_shared<TextureSDL>();
    
    if (!texture->load(file))
    {
        return nullptr;
    }

    if (!texture->loadTexture(*this))
    {
        return nullptr;
    }

    return std::static_pointer_cast<Texture>(texture);
}

void RendererSDL::draw(
    std::shared_ptr<Texture> t, 
    const Vec2f& position, 
    float angle, 
    const Vec2f& scale,
    const Vec2f& origin
)
{
    Sizei ts = t->getSize();

    draw(
        t, 
        position,
        angle,
        scale,
        origin,
        Vec4i { 0, 0, ts.x, ts.y }
    );
}

void RendererSDL::draw(
    std::shared_ptr<Texture> t, 
    const Vec2f& position, 
    float angle, 
    const Vec2f& scale,
    const Vec2f& origin,
    const Vec4i& rect
)
{
    assert(t);
    assert(this->renderer);

    std::shared_ptr<TextureSDL> sdlTex = std::static_pointer_cast<TextureSDL>(t);

    SDL_Rect srcRect = { 
        static_cast<int>(rect.x), 
        static_cast<int>(rect.y), 
        static_cast<int>(rect.z), 
        static_cast<int>(rect.w)
    };

    float w = rect.z * scale.x;
    float h = rect.w * scale.y;
    float x = position.x - this->cameraPos.x - origin.x * scale.x;
    float y = position.y - this->cameraPos.y - origin.y * scale.y;

    SDL_Rect dstRect = {
        static_cast<int>(x), 
        static_cast<int>(y), 
        static_cast<int>(w),
        static_cast<int>(h) 
    };

    SDL_Point rotOrigin = { 
        static_cast<int>(origin.x * scale.x),
        static_cast<int>(origin.y * scale.y)
    };
    
    SDL_RenderCopyEx(
        this->renderer,
        sdlTex->texture,
        &srcRect,
        &dstRect,
        angle,
        &rotOrigin,
        SDL_FLIP_NONE
    );
}

void RendererSDL::clear()
{
    assert(this->renderer);
    SDL_RenderClear(this->renderer);
}

void RendererSDL::flip()
{
    assert(this->renderer);
    SDL_RenderPresent(this->renderer);
}

void RendererSDL::setCameraPosition(const Vec2f& pos)
{
    this->cameraPos = pos;
}

void RendererSDL::setCameraZoom(float zoom)
{
    assert(this->renderer);
    SDL_RenderSetScale(this->renderer, zoom, zoom);
    this->cameraZoom = zoom;
}

Vec2f RendererSDL::getCameraPosition()
{
    return this->cameraPos;
}

float RendererSDL::getCameraZoom()
{
    return this->cameraZoom;
}

Vec2i RendererSDL::getResolution()
{
    assert(this->renderer);

    int w, h;
    SDL_GetRendererOutputSize(this->renderer, &w, &h);

    return { w, h };
}

} // namespace graphics
} // namespace kola