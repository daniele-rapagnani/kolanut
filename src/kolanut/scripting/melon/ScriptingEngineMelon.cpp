#include "kolanut/core/Logging.h"
#include "kolanut/scripting/melon/ScriptingEngineMelon.h"
#include "kolanut/scripting/melon/modules/KolanutModule.h"

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

Value* getValueFromModule(
    VM& vm, 
    TType type, 
    const std::string& module, 
    const std::string& key
)
{
    GCItem* moduleStr = melNewString(&vm, module.c_str(), module.size());
    melM_vstackPushGCItem(&vm.stack, moduleStr);

    GCItem* keyStrs = melNewString(&vm, key.c_str(), key.size());
    melM_vstackPushGCItem(&vm.stack, keyStrs);

    Value* val = melGetTypeFromModule(
        &vm, 
        melM_stackOffset(&vm.stack, 1), 
        melM_stackTop(&vm.stack), 
        type
    );

    melM_stackPopCount(&vm.stack, 2);

    return val;
}

template <typename T>
TByte push(VM* vm, const T& val) = delete;

template <>
TByte push<double>(VM* vm, const double& val)
{
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* stackVal = melM_stackAllocRaw(&vm->stack);
    stackVal->type = MELON_TYPE_NUMBER;
    stackVal->pack.value.number = val;

    return 1;
}

template <>
TByte push<float>(VM* vm, const float& val)
{
    return push<double>(vm, static_cast<double>(val));
}

void callClosure(VM& vm, Value* closure)
{
    assert(closure->type == MELON_TYPE_CLOSURE);

    melM_stackPush(&vm.stack, closure);
    melCallClosureSyncVM(&vm, 0, 0, 0);
}

template <typename ...Types>
void callClosure(VM& vm, Value* closure, Types ...args)
{
    assert(closure->type == MELON_TYPE_CLOSURE);

    std::function<void(TByte)> dummy = [] (TByte total) {};

    melM_stackPush(&vm.stack, closure);
    dummy(push(&vm, std::forward<Types>(args))...);
    melCallClosureSyncVM(&vm, sizeof...(Types), 0, 0);
}

void callModuleClosure(VM& vm, const std::string& module, const std::string& key)
{
    Value* cl = getValueFromModule(
        vm, 
        MELON_TYPE_CLOSURE,
        module,
        key
    );

    if (!cl)
    {
        knM_logError("Can't find function '" << key << "' in module '" << module << "'");
        return;
    }

    callClosure(vm, cl);
}

template <typename ...Types>
void callModuleClosure(VM& vm, const std::string& module, const std::string& key, Types ...args)
{
    Value* cl = getValueFromModule(
        vm, 
        MELON_TYPE_CLOSURE,
        module,
        key
    );

    if (!cl)
    {
        knM_logError("Can't find function '" << key << "' in module '" << module << "'");
        return;
    }

    callClosure(vm, cl, args...);
}

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

    std::string bootScript = config.scriptsDir + "/" + config.bootScript + ".ms";

    if (!melon::utils::runScript(&this->vm, bootScript))
    {
        knM_logFatal("Can't load boot script at: '" << bootScript << "'");
        return false;
    }

    return true;
}

void ScriptingEngineMelon::onLoad()
{
    callModuleClosure(vm, "kolanut", "onLoad");
}

void ScriptingEngineMelon::onLoop(float dt)
{
    callModuleClosure(vm, "kolanut", "onLoop", dt);
}

} // namespace scripting
} // namespace kola