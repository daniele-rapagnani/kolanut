#include "kolanut/core/Logging.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/graphics/Texture.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/scripting/melon/bindings/Texture.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/Modules.h"

#include <cassert>

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

TByte draw(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, x, MELON_TYPE_NUMBER, 0);
    melM_arg(vm, y, MELON_TYPE_NUMBER, 1);
    melM_argOptional(vm, angle, MELON_TYPE_NUMBER, 2);
    melM_argOptional(vm, sx, MELON_TYPE_NUMBER, 3);
    melM_argOptional(vm, sy, MELON_TYPE_NUMBER, 4);

    float anglef = angle->type == MELON_TYPE_NULL ? 0.0f : angle->pack.value.number;
    float sxf = sx->type == MELON_TYPE_NULL ? 1.0f : sx->pack.value.number;
    float syf = sy->type == MELON_TYPE_NULL ? 1.0f : sy->pack.value.number;

    std::shared_ptr<graphics::Texture> tex = 
        ffi::getInstance<graphics::Texture>(vm, thisObj->pack.obj)
    ;

    di::get<graphics::Renderer>()->draw(
        tex, { x->pack.value.number, y->pack.value.number }, anglef, { sxf, syf }
    );

    return 0;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "draw", 7, 0, &draw, 1 },
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