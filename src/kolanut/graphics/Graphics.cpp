
#include "kolanut/core/Logging.h"
#include "kolanut/graphics/Graphics.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/graphics/sdl/RendererSDL.h"

#include <cassert>

namespace kola {
namespace graphics {

std::shared_ptr<Renderer> createRenderer(const Config& conf)
{
    switch(conf.renderer)
    {
        case Engine::SDL2:
            return std::make_shared<RendererSDL>();
        
        default:
            assert(false);
            knM_logFatal("Unknown renderer: " << static_cast<int>(conf.renderer));
    }

    return nullptr;
}

} // namespace graphics
} // namespace kola