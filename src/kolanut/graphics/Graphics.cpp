
#include "kolanut/core/Logging.h"
#include "kolanut/graphics/Graphics.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/graphics/ogl/RendererOGL.h"
#include "kolanut/graphics/vulkan/RendererVK.h"

#include <cassert>

namespace kola {
namespace graphics {

std::shared_ptr<Renderer> createRenderer(const Config& conf)
{
    switch(conf.renderer)
    {
        case Engine::OGL:
            return std::make_shared<RendererOGL>();

        case Engine::VULKAN:
            return std::make_shared<RendererVK>();
        
        default:
            assert(false);
            knM_logFatal("Unknown renderer: " << static_cast<int>(conf.renderer));
    }

    return nullptr;
}

} // namespace graphics
} // namespace kola