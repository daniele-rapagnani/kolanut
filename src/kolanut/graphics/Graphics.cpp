
#include "kolanut/core/Logging.h"
#include "kolanut/graphics/Graphics.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/graphics/ogl/RendererOGL.h"
#include "kolanut/graphics/vulkan/RendererVK.h"

#include <unordered_map>
#include <cassert>

namespace kola {
namespace graphics {

namespace {

std::unordered_map<std::string, Engine> ENGINE_NAMES = {
    { "opengl", Engine::OGL },
    { "vulkan", Engine::VULKAN },
};

} // namespace

Engine engineFromString(const std::string& s)
{
    if (ENGINE_NAMES.find(s) == ENGINE_NAMES.end())
    {
        return Engine::NONE;
    }

    return ENGINE_NAMES.at(s);
}

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