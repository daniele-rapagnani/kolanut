#pragma once

#include <memory>

namespace kola {
namespace scripting {

class ScriptingEngine;

enum class Engine
{
    NONE = 0,
    MELON
};

struct Config
{
    Engine scripting = Engine::MELON;
};

std::shared_ptr<ScriptingEngine> createScriptingEngine(const Config& config);

} // namespace scripting
} // namespace kola