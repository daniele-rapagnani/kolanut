#pragma once

#include "kolanut/graphics/Graphics.h"
#include "kolanut/scripting/Scripting.h"
#include "kolanut/events/Events.h"

#include <memory>

namespace kola {

class Kolanut
{
public:
    struct Config
    {
        graphics::Config graphics;
        scripting::Config scripting;
        events::Config events;
    };

public:
    bool init(const Config& conf);
    void run();

private:
    std::shared_ptr<graphics::Renderer> renderer = nullptr;
    std::shared_ptr<scripting::ScriptingEngine> scripting = nullptr;
    std::shared_ptr<events::EventSystem> eventSystem = nullptr;
};

} // namespace kola