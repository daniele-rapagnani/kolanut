#pragma once

#include "kolanut/events/EventSystem.h"

namespace kola {
namespace events {

class EventSystemSDL : public EventSystem
{ 
public:
    bool init(const Config& conf) override;
    bool poll() override;
    uint64_t getTimeMS() override;
    bool isKeyPressed(KeyCode key) override;

private:
    bool keysPressedMap[static_cast<unsigned int>(KeyCode::KEY_COUNT)] = {0};
};

} // namespace events
} // namespace kola