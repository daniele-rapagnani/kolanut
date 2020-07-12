#include "kolanut/events/sdl/EventSystemSDL.h"

#include <SDL.h>

namespace kola {
namespace events {

bool EventSystemSDL::init(const Config& conf)
{
    return true;
}

bool EventSystemSDL::poll()
{
    SDL_Event event;

    if (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            if (getQuitCallback())
            {
                getQuitCallback()();
            }
        }
    }

    return true;
}

uint64_t EventSystemSDL::getTimeMS()
{
    return SDL_GetTicks();
}

} // namespace events
} // namespace kola