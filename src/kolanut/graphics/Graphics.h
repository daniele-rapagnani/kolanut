#pragma once

#include "kolanut/core/BuildConfig.h"
#include "kolanut/core/Types.h"

#include <memory>
#include <string>

namespace kola {
namespace graphics {

class Renderer;

enum class Engine
{
    NONE = 0,
    OGL
#if defined(_ENABLE_VULKAN)
    ,VULKAN
#endif
};

enum ResolutionFitMode
{
    NONE = 0,
    CONTAIN,
    COVER,
    STRETCH
};

struct Resolution
{
    ResolutionFitMode resizeMode = ResolutionFitMode::CONTAIN;
    Sizei screenSize = { 800, 600 };
    Sizei designResolution = { 800, 600 };
    bool fullScreen = false;
    bool vSynced = true;
};

struct Config
{
    Engine renderer = Engine::OGL;
    std::string windowTitle;
    Resolution resolution;
    bool enableAPIDebug = false;
    uint8_t framesInFlight = 2;
    size_t maxGeometryBufferVertices = 20000 * 6;
    size_t jobQueueInitialSize = 5000;
    std::string forceGPU = {};
};

Engine engineFromString(const std::string& s);
ResolutionFitMode resolutionFitModeFromString(const std::string& s);
std::shared_ptr<Renderer> createRenderer(const Config& conf);

} // namespace graphics
} // namespace kola