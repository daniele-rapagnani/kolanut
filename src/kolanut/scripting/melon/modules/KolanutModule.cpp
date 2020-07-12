#include "kolanut/scripting/melon/modules/KolanutModule.h"

extern "C" {
#include <melon/modules/modules.h>
}

extern "C" {

static TByte onLoad(VM* vm)
{
    melM_fatal(vm, "Please replace the onLoad function in the kolanut module.");
    return 0;
}

static TByte onLoop(VM* vm)
{
    melM_fatal(vm, "Please replace the onLoop function in the kolanut module.");
    return 0;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "onLoad", 0, 0, &onLoad },
    { "onLoop", 0, 0, &onLoop },
    { NULL, 0, 0, NULL }
};

TRet kolanutModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}

} // extern "C"