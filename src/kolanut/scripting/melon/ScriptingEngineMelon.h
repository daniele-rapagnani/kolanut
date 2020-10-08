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
    void onLoad() override;
    void onUpdate(float dt) override;
    void onDraw() override;
    void onDrawUI() override;
    bool onQuit() override;
    void onKeyPressed(events::KeyCode key, bool pressed) override;

private:
    VMConfig vmConfig;
    VM vm;
};

} // namespace scripting
} // namespace kola