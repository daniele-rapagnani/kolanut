#include "kolanut/core/Logging.h"
#include "kolanut/scripting/melon/ScriptingEngineMelon.h"
#include "kolanut/scripting/melon/modules/KolanutModule.h"
#include "kolanut/scripting/melon/ffi/FFI.h"
#include "kolanut/scripting/melon/bindings/Bindings.h"

#include <melon/tools/utils.h>

extern "C" {
#include <melon/core/tstring.h>
#include <melon/modules/modules.h>
}

#include <cassert>

namespace kola {
namespace scripting {

namespace {

Module KOLANUT_MODULES[] = {
    { "kolanut", kolanutModuleInit },
    { NULL, NULL }
};

} // namesace

bool ScriptingEngineMelon::init(const Config& config)
{
    knM_logDebug("Initilizing Melon scripting");

	if (melCreateVM(&this->vm, &this->vmConfig) != 0)
	{
		knM_logFatal("Can't create melon VM");
		return false;
	}

    if (melRegisterModules(&this->vm, KOLANUT_MODULES) != 0)
    {
        knM_logFatal("Can't init kolanut's melon modules");
        return false;
    }

    melon::bindings::registerBindings(&this->vm);

    std::string bootScript = config.scriptsDir + "/" + config.bootScript + ".ms";

    if (!::melon::utils::runScript(&this->vm, bootScript))
    {
        knM_logFatal("Can't load boot script at: '" << bootScript << "'");
        return false;
    }

    return true;
}

void ScriptingEngineMelon::onLoad()
{
    melon::ffi::callModuleClosure(vm, "kolanut", "onLoad");
}

void ScriptingEngineMelon::onUpdate(float dt)
{
    melon::ffi::callModuleClosure(vm, "kolanut", "onUpdate", dt);
}

void ScriptingEngineMelon::onDraw()
{
    melon::ffi::callModuleClosure(vm, "kolanut", "onDraw");
}

bool ScriptingEngineMelon::onQuit()
{
    bool res = true;
    melon::ffi::callModuleClosureRet(vm, "kolanut", "onQuit", res);
    return res;
}

} // namespace scripting
} // namespace kola