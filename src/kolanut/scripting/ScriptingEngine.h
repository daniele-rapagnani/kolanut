#pragma once

#include "kolanut/core/Kolanut.h"
#include "kolanut/scripting/Scripting.h"
#include "kolanut/stats/StatsEngine.h"
#include "kolanut/events/KeyCodes.h"

namespace kola {
namespace scripting {

class ScriptingEngine
{
public:
    virtual bool init(const Config& config) = 0;
    virtual bool loadConfig(Kolanut::Config& config) = 0;
    virtual bool start() = 0;
    virtual void onLoad() = 0;
    virtual void onUpdate(float dt) = 0;
    virtual void onDraw() = 0;
    virtual void onDrawUI() = 0;
    virtual void onStatsUpdated(const stats::StatsEngine::Result& result) = 0;
    virtual bool onQuit() = 0;
    virtual void onKeyPressed(events::KeyCode key, bool pressed) = 0;
};

} // namespace scripting
} // namespace kola