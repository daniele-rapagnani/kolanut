#include "kolanut/core/Logging.h"
#include "kolanut/scripting/Scripting.h"
#include "kolanut/scripting/melon/ScriptingEngineMelon.h"

namespace kola {
namespace scripting {

std::shared_ptr<ScriptingEngine> createScriptingEngine(const Config& config)
{
    switch(config.scripting)
    {
        case Engine::MELON:
            return std::make_shared<ScriptingEngineMelon>();
        
        default:
            assert(false);
            knM_logFatal("Unknown scripting engine: " << static_cast<int>(config.scripting));
    }

    return nullptr;
}

} // namespace scripting
} // namespace kola