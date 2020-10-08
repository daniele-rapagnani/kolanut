#pragma once

#include "kolanut/scripting/Scripting.h"
#include "kolanut/events/KeyCodes.h"

namespace kola {
namespace scripting {

class ScriptingEngine
{
public:
    virtual bool init(const Config& config) = 0;
    virtual void onLoad() = 0;
    virtual void onUpdate(float dt) = 0;
    virtual void onDraw() = 0;
    virtual void onDrawUI() = 0;
    virtual bool onQuit() = 0;
    virtual void onKeyPressed(events::KeyCode key, bool pressed) = 0;
};

} // namespace scripting
} // namespace kola