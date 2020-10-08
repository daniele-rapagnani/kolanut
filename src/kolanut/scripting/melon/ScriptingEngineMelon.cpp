#include "kolanut/core/Logging.h"
#include "kolanut/scripting/melon/ScriptingEngineMelon.h"
#include "kolanut/scripting/melon/modules/KolanutModule.h"
#include "kolanut/scripting/melon/modules/VectorModule.h"
#include "kolanut/scripting/melon/modules/SpriteModule.h"
#include "kolanut/scripting/melon/modules/RectModule.h"
#include "kolanut/scripting/melon/modules/ColorModule.h"
#include "kolanut/scripting/melon/ffi/FFI.h"
#include "kolanut/scripting/melon/ffi/Modules.h"
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
    { "Vector", vectorModuleInit },
    { "Sprite", spriteModuleInit },
    { "Rect", rectModuleInit },
    { "Color", colorModuleInit },
    { "Kolanut", kolanutModuleInit },
    { NULL, NULL }
};

#include "kolanut/scripting/melon/KeyCodesNames.inl"

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
    melon::ffi::callModuleClosure(vm, "Kolanut", "onLoad");
}

void ScriptingEngineMelon::onUpdate(float dt)
{
    melon::ffi::callModuleClosure(vm, "Kolanut", "onUpdate", dt);
}

void ScriptingEngineMelon::onDraw()
{
    melon::ffi::callModuleClosure(vm, "Kolanut", "onDraw");
}

void ScriptingEngineMelon::onDrawUI()
{
    melon::ffi::callModuleClosure(vm, "Kolanut", "onDrawUI");
}

bool ScriptingEngineMelon::onQuit()
{
    bool res = true;
    melon::ffi::callModuleClosureRet(vm, "Kolanut", "onQuit", res);
    return res;
}

void ScriptingEngineMelon::onKeyPressed(events::KeyCode key, bool pressed)
{
    const char* keyName = KEYCODE_NAMES[static_cast<unsigned int>(key)];
    melon::ffi::callModuleClosure(vm, "Kolanut", "onKeyPressed", keyName, pressed);
}

} // namespace scripting
} // namespace kola