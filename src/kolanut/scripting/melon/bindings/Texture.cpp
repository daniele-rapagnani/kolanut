#include "kolanut/core/Logging.h"
#include "kolanut/scripting/melon/bindings/Texture.h"
#include "kolanut/scripting/melon/ffi/Modules.h"

#include <cassert>

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

TByte test(VM* vm)
{
    knM_logDebug("CAZZ");
    return 0;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "test", 0, 0, &test },
    { NULL, 0, 0, NULL }
};

}

void registerTextureBindings(VM* vm)
{
    Value* val = ffi::getModule(*vm, "kolanut");
    assert(val);

    Value key;
    key.type = MELON_TYPE_STRING;
    key.pack.obj = melNewString(vm, "Texture", strlen("Texture"));
    melM_stackPush(&vm->stack, &key);

    if (melNewModule(vm, funcs) != 0)
    {
        knM_logError("Can't create module for Texture");
        return;
    }

    Value* mod = melM_stackTop(&vm->stack);
    melSetValueObject(vm, val->pack.obj, &key, mod);

    // Pop key and module
    melM_stackPopCount(&vm->stack, 2);
}

} // namespace binding
} // namespace melon
} // namespace kola