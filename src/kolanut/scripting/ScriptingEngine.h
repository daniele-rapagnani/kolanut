#pragma once

#include "kolanut/scripting/Scripting.h"

namespace kola {
namespace scripting {

class ScriptingEngine
{
public:
    virtual bool init(const Config& config) = 0;
    virtual void onLoad() = 0;
    virtual void onLoop(float dt) = 0;
};

} // namespace scripting
} // namespace kola