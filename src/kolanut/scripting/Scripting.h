#pragma once

#include <memory>
#include <string>

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
    std::string scriptsDir = "assets/scripts";
    std::string bootScript = "boot";
    std::string configScript = "config";
};

std::shared_ptr<ScriptingEngine> createScriptingEngine(const Config& config);

} // namespace scripting
} // namespace kola