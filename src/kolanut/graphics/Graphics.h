#pragma once

#include "kolanut/core/Types.h"

#include <memory>
#include <string>

namespace kola {
namespace graphics {

class Renderer;

enum class Engine
{
    NONE = 0,
    SDL2,
    OGL
};

struct Resolution
{
    Sizei screenSize = { 800, 600 };
    Sizei designResolution = { 800, 600 };
    bool fullScreen = false;
    bool vSynced = true;
};

struct Config
{
    Engine renderer = Engine::SDL2;
    std::string windowTitle;
    Resolution resolution;
};

std::shared_ptr<Renderer> createRenderer(const Config& conf);

} // namespace graphics
} // namespace kola