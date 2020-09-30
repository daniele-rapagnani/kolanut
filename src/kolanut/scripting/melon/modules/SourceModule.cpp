#include "kolanut/scripting/melon/modules/SourceModule.h"
#include "kolanut/core/Logging.h"

#include <cassert>

extern "C" {
    #include <melon/core/tstring.h>
    #include <melon/modules/modules.h>
}

extern "C" {

TRet sourceModuleInit(VM* vm, const char* path, const char* source)
{
    melSaveAndPauseGC(vm, &vm->gc);

    GCItem* func = melCompileSourceModule(
        vm, 
        melNewString(vm, path, strlen(path)),
        melNewString(vm, source, strlen(source)),
        nullptr
    );

    // The compiled function can still be collected by melRunModuleFunctionVM
    melM_vstackPushGCItem(&vm->stack, func);
    
    if (!func || func->type != MELON_TYPE_FUNCTION)
    {
        knM_logFatal("Can't build builtin module: '" << path << "'");
        return 1;
    }

    melRestorePauseGC(vm, &vm->gc);

    if (melRunModuleFunctionVM(vm, func) != 0)
    {
        knM_logFatal("Can't execute builtin module: '" << path << "'");
        return 1;
    }

    Value* obj = melM_stackTop(&vm->stack);

    if (obj->type != MELON_TYPE_OBJECT)
    {
        knM_logFatal("Builtin module '" << path << "' didn't return a valid object");
        return 1;
    }

    // Remove the saved function and overwrite it with the module result
    *(melM_stackOffset(&vm->stack, 1)) = *obj;
    melM_stackPop(&vm->stack);

    return 0;
}

} // extern "C"