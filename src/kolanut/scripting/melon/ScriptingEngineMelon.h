#pragma once

#include "kolanut/scripting/ScriptingEngine.h"

extern "C" {
#include <melon/core/vm.h>
}

namespace kola {
namespace scripting {

class ScriptingEngineMelon : public ScriptingEngine
{
public:
    bool init(const Config& config) override;
    bool loadConfig(Kolanut::Config& config) override;
    bool start() override;
    void onLoad() override;
    void onUpdate(float dt) override;
    void onDraw() override;
    void onDrawUI() override;
    void onStatsUpdated(const stats::StatsEngine::Result& result) override;
    bool onQuit() override;
    void onKeyPressed(events::KeyCode key, bool pressed) override;

private:
    std::string getScriptPath(const std::string& name) const
    {
        return this->config.scriptsDir + "/" + name + ".ms";
    }

private:
    Config config = {};
    VMConfig vmConfig = {};
    VM vm = {};
};

} // namespace scripting
} // namespace kola