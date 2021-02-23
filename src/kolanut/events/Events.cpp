
#include "kolanut/core/Logging.h"
#include "kolanut/events/EventSystem.h"
#include "kolanut/events/glfw/EventSystemGLFW.h"

#include <unordered_map>
#include <cassert>

namespace kola {
namespace events {

namespace {

std::unordered_map<std::string, Engine> ENGINE_NAMES = {
    { "glfw", Engine::GLFW },
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

std::shared_ptr<EventSystem> createEventSystem(const Config& conf)
{
    switch(conf.eventSystem)
    {
        case Engine::GLFW:
            return std::make_shared<EventSystemGLFW>();
        
        default:
            assert(false);
            knM_logFatal("Unknown event system: " << static_cast<int>(conf.eventSystem));
    }

    return nullptr;
}

} // namespace events
} // namespace kola