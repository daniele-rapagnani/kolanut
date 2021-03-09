#include "kolanut/core/Logging.h"
#include "kolanut/scripting/melon/ScriptingEngineMelon.h"
#include "kolanut/scripting/melon/modules/KolanutModule.h"
#include "kolanut/scripting/melon/modules/Vector2Module.h"
#include "kolanut/scripting/melon/modules/Vector3Module.h"
#include "kolanut/scripting/melon/modules/Vector4Module.h"
#include "kolanut/scripting/melon/modules/RectModule.h"
#include "kolanut/scripting/melon/modules/ColorModule.h"
#include "kolanut/scripting/melon/ffi/FFI.h"
#include "kolanut/scripting/melon/ffi/Modules.h"
#include "kolanut/scripting/melon/ffi/PopVectors.h"
#include "kolanut/scripting/melon/bindings/Bindings.h"

#include <melon/tools/utils.h>

extern "C" {
#include <melon/core/tstring.h>
#include <melon/modules/modules.h>
}

#include <cassert>

namespace kola {

namespace melon {
namespace ffi {

template <>
inline bool convert(VM* vm, Kolanut::Config& res, Value* val)
{
    if (val->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    getInstanceEnumField<graphics::Engine>(
        vm, 
        val->pack.obj,
        "graphicBackend", 
        res.graphics.renderer, 
        graphics::engineFromString
    );

    getInstanceEnumField<events::Engine>(
        vm, 
        val->pack.obj, 
        "eventSystem", 
        res.events.eventSystem, 
        events::engineFromString
    );

    getInstanceField(vm, val->pack.obj, "windowTitle", res.graphics.windowTitle);
    getInstanceField(vm, val->pack.obj, "screenSize", res.graphics.resolution.screenSize);
    getInstanceField(vm, val->pack.obj, "designResolution", res.graphics.resolution.designResolution);
    getInstanceField(vm, val->pack.obj, "fullscreen", res.graphics.resolution.fullScreen);
    getInstanceField(vm, val->pack.obj, "vsync", res.graphics.resolution.vSynced);
    getInstanceField(vm, val->pack.obj, "enableGraphicAPIDebug", res.graphics.enableAPIDebug);
    getInstanceField(vm, val->pack.obj, "framesInFlight", res.graphics.framesInFlight);
    getInstanceField(vm, val->pack.obj, "maxGeometryBufferVertices", res.graphics.maxGeometryBufferVertices);
    getInstanceField(vm, val->pack.obj, "jobQueueInitialSize", res.graphics.jobQueueInitialSize);
    getInstanceField(vm, val->pack.obj, "forceGPU", res.graphics.forceGPU);
    getInstanceField(vm, val->pack.obj, "enableStats", res.enableStats);

    return true;
}

template <>
class Push<stats::StatsEngine::Result>
{
public:
    static TByte push(VM* vm, const stats::StatsEngine::Result& val)
    {
        GCItem* obj = melNewObject(vm);
        melM_vstackPushGCItem(&vm->stack, obj);

        setInstanceField(vm, obj, "label", *val.label);
        setInstanceField(vm, obj, "avg", val.avg);
        setInstanceField(vm, obj, "total", val.total);
        setInstanceField(vm, obj, "samplesCount", val.samplesCount);

        return 1;
    }
};

} // namespace ffi
} // namespace melon

namespace scripting {

namespace {

Module KOLANUT_MODULES[] = {
    { "Vector2", vector2ModuleInit },
    { "Vector3", vector3ModuleInit },
    { "Vector4", vector4ModuleInit },
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

    this->config = config;

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

    return true;
}

bool ScriptingEngineMelon::start()
{
    std::string bootScript = getScriptPath(this->config.bootScript);

    if (!::melon::utils::runScript(&this->vm, bootScript))
    {
        knM_logFatal("Can't load boot script at: '" << bootScript << "'");
        return false;
    }

    return true;
}

bool ScriptingEngineMelon::loadConfig(Kolanut::Config& config)
{
    return melon::ffi::callModuleClosureRet(
        this->vm, 
        "import", 
        &MELON_SYMBOLIC_KEYS[MELON_OBJSYM_CALL],
        config,
        getScriptPath(this->config.configScript)
    );
}

void ScriptingEngineMelon::onLoad()
{
    melon::ffi::callModuleClosure(this->vm, "Kolanut", "onLoad");
}

void ScriptingEngineMelon::onUpdate(float dt)
{
    melon::ffi::callModuleClosure(this->vm, "Kolanut", "onUpdate", dt);
}

void ScriptingEngineMelon::onDraw()
{
    melon::ffi::callModuleClosure(this->vm, "Kolanut", "onDraw");
}

void ScriptingEngineMelon::onDrawUI()
{
    melon::ffi::callModuleClosure(this->vm, "Kolanut", "onDrawUI");
}

void ScriptingEngineMelon::onStatsUpdated(const stats::StatsEngine::Result& result)
{
    melon::ffi::callModuleClosure(this->vm, "Kolanut", "onStatsUpdated", result);
}

bool ScriptingEngineMelon::onQuit()
{
    bool res = true;
    melon::ffi::callModuleClosureRet(this->vm, "Kolanut", "onQuit", res);
    return res;
}

void ScriptingEngineMelon::onKeyPressed(events::KeyCode key, bool pressed)
{
    const char* keyName = KEYCODE_NAMES[static_cast<unsigned int>(key)];
    melon::ffi::callModuleClosure(this->vm, "Kolanut", "onKeyPressed", keyName, pressed);
}

} // namespace scripting
} // namespace kola