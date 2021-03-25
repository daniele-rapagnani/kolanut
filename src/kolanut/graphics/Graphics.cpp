
#include "kolanut/core/Logging.h"
#include "kolanut/graphics/Graphics.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/graphics/ogl/RendererOGL.h"

#if defined(_ENABLE_VULKAN)
#include "kolanut/graphics/vulkan/RendererVK.h"
#endif

#include <unordered_map>
#include <cassert>

namespace kola {
namespace graphics {

namespace {

std::unordered_map<std::string, Engine> ENGINE_NAMES = {
    { "opengl", Engine::OGL },
#if defined(_ENABLE_VULKAN)
    { "vulkan", Engine::VULKAN },
#endif
};

std::unordered_map<std::string, ResolutionFitMode> RESOLUTION_FIT_NAMES = {
    { "contain", ResolutionFitMode::CONTAIN },
    { "cover", ResolutionFitMode::COVER },
    { "stretch", ResolutionFitMode::STRETCH },
};

template <typename T>
T enumFromString(
    const std::string& s, 
    const std::unordered_map<std::string, T>& map,
    T defaultVal
)
{
    if (map.find(s) == map.end())
    {
        return defaultVal;
    }

    return map.at(s);
}

} // namespace

Engine engineFromString(const std::string& s)
{
    return enumFromString(s, ENGINE_NAMES, Engine::NONE);
}

ResolutionFitMode resolutionFitModeFromString(const std::string& s)
{
    return enumFromString(s, RESOLUTION_FIT_NAMES, ResolutionFitMode::NONE);
}

std::shared_ptr<Renderer> createRenderer(const Config& conf)
{
    switch(conf.renderer)
    {
        case Engine::OGL:
            return std::make_shared<RendererOGL>();

#if defined(_ENABLE_VULKAN)
        case Engine::VULKAN:
            return std::make_shared<RendererVK>();
#endif
        
        default:
            assert(false);
            knM_logFatal("Unknown renderer: " << static_cast<int>(conf.renderer));
    }

    return nullptr;
}

} // namespace graphics
} // namespace kola