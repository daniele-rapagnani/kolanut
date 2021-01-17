#pragma once

#include "kolanut/graphics/Graphics.h"
#include "kolanut/scripting/Scripting.h"
#include "kolanut/events/Events.h"
#include "kolanut/filesystem/Filesystem.h"
#include "kolanut/core/DIContainer.h"

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
        filesystem::Config filesystem;
    };

public:
    bool init(const Config& conf);
    void run();
    
    const Config& getConfig() const
    { return this->config; }
    
protected:
    std::shared_ptr<events::EventSystem> getEventSystem() const;
    std::shared_ptr<scripting::ScriptingEngine> getScriptingEngine() const;
    std::shared_ptr<graphics::Renderer> getRenderer() const;

    Config config;
};

} // namespace kola