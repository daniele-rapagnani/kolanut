
#include "kolanut/core/Logging.h"
#include "kolanut/events/EventSystem.h"
#include "kolanut/events/sdl/EventSystemSDL.h"

#include <cassert>

namespace kola {
namespace events {

std::shared_ptr<EventSystem> createEventSystem(const Config& conf)
{
    switch(conf.eventSystem)
    {
        case Engine::SDL2:
            return std::make_shared<EventSystemSDL>();
        
        default:
            assert(false);
            knM_logFatal("Unknown event system: " << static_cast<int>(conf.eventSystem));
    }

    return nullptr;
}

} // namespace events
} // namespace kola