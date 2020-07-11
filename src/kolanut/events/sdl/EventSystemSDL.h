#pragma once

#include "kolanut/events/EventSystem.h"

namespace kola {
namespace events {

class EventSystemSDL : public EventSystem
{ 
public:
    bool init(const Config& conf) override;
    bool poll() override;
};

} // namespace events
} // namespace kola