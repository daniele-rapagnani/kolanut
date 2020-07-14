#include "kolanut/events/sdl/EventSystemSDL.h"

#include <SDL.h>

#include <cassert>
#include <unordered_map>

namespace kola {
namespace events {

namespace {
#include "kolanut/events/sdl/KeyCodesMapSDL.inl"
}

bool EventSystemSDL::init(const Config& conf)
{
    return true;
}

bool EventSystemSDL::poll()
{
    SDL_Event event;

    if (SDL_PollEvent(&event))
    {
        if (event.type == SDL_KEYUP || event.type == SDL_KEYDOWN)
        {
            assert(SDL_KOLA_KEYMAP.find(event.key.keysym.sym) != SDL_KOLA_KEYMAP.end());
            KeyCode keyCode = SDL_KOLA_KEYMAP[event.key.keysym.sym];

            bool& val = this->keysPressedMap[static_cast<unsigned int>(keyCode)];
            
            bool prevValue = val;
            bool curValue = event.type == SDL_KEYDOWN;
            val = curValue;

            if (prevValue != curValue && getKeyPressedCallback())
            {
                getKeyPressedCallback()(keyCode, curValue);
            }
        }
        else if (event.type == SDL_QUIT)
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

bool EventSystemSDL::isKeyPressed(KeyCode key)
{
    return this->keysPressedMap[static_cast<unsigned int>(key)];
}

} // namespace events
} // namespace kola