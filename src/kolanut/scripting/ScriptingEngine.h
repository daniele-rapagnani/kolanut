#pragma once

#include "kolanut/scripting/Scripting.h"

namespace kola {
namespace scripting {

class ScriptingEngine
{
public:
    virtual bool init(const Config& config) = 0;
    virtual void onLoad() = 0;
    virtual void onUpdate(float dt) = 0;
    virtual void onDraw() = 0;
    virtual bool onQuit() = 0;
};

} // namespace scripting
} // namespace kola