#pragma once

#include "kolanut/core/Types.h"
#include <memory>

namespace kola {
namespace events {

class EventSystem;

enum class Engine
{
    NONE = 0,
    GLFW
};

struct Config
{
    Engine eventSystem = Engine::GLFW;
};

Engine engineFromString(const std::string& s);
std::shared_ptr<EventSystem> createEventSystem(const Config& conf);

} // namespace events
} // namespace kola