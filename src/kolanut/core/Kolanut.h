#pragma once

#include "kolanut/graphics/Graphics.h"
#include "kolanut/scripting/Scripting.h"
#include "kolanut/events/Events.h"
#include "kolanut/filesystem/Filesystem.h"
#include "kolanut/stats/StatsEngine.h"
#include "kolanut/audio/Audio.h"
#include "kolanut/core/DIContainer.h"

#include <memory>

namespace kola {

class Kolanut
{
public:
    struct BoostrapConfig
    {
        filesystem::Config filesystem;
        scripting::Config scripting;
    };

    struct Config
    {
        graphics::Config graphics;
        events::Config events;
        audio::Config audio;
        bool enableStats = false;
    };

public:
    bool bootstrap(const BoostrapConfig& conf);
    bool loadConfig(Config& conf);
    bool init(const Config& conf);
    void run();
    
    const BoostrapConfig& getBootstrapConfig() const
    { return this->bootstrapConfig; }

    const Config& getConfig() const
    { return this->config; }

    void renderFrame();

protected:
    std::shared_ptr<events::EventSystem> getEventSystem() const;
    std::shared_ptr<scripting::ScriptingEngine> getScriptingEngine() const;
    std::shared_ptr<graphics::Renderer> getRenderer() const;
    std::shared_ptr<stats::StatsEngine> getStatsEngine() const;

    Config config = {};
    BoostrapConfig bootstrapConfig = {};
    uint64_t lastFrameTime = 0;

    // @todo: Initial dt is 60Hz hardcoded
    float dt = 1.0/60.0f;
};

} // namespace kola