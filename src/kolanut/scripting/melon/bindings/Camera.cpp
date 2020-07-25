#include "kolanut/core/Logging.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/graphics/Texture.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/scripting/melon/bindings/Camera.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/Modules.h"
#include "kolanut/scripting/melon/ffi/PopVectors.h"
#include "kolanut/scripting/melon/ffi/PushVectors.h"

#include <cassert>

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

TByte setPosition(VM* vm)
{
    melM_arg(vm, posVal, MELON_TYPE_OBJECT, 0);

    Vec2f pos;
    ffi::convert(vm, pos, posVal);

    di::get<graphics::Renderer>()->setCameraPosition(pos);
    return 0;
}

TByte getPosition(VM* vm)
{
    Vec2f cPos = di::get<graphics::Renderer>()->getCameraPosition();
    return ffi::push(vm, cPos);
}

TByte setZoom(VM* vm)
{
    melM_arg(vm, zoomVal, MELON_TYPE_NUMBER, 0);
    di::get<graphics::Renderer>()->setCameraZoom(zoomVal->pack.value.number);
    return 0;
}

TByte getZoom(VM* vm)
{
    float zoom = di::get<graphics::Renderer>()->getCameraZoom();
    return ffi::push(vm, zoom);
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "setPosition", 1, 0, &setPosition, 0 },
    { "getPosition", 0, 0, &getPosition, 0 },
    { "setZoom", 1, 0, &setZoom, 0 },
    { "getZoom", 0, 0, &getZoom, 0 },
    { NULL, 0, 0, NULL }
};

}

void registerCameraBindings(VM* vm)
{
    Value* val = ffi::getModule(*vm, "Kolanut");
    assert(val);

    Value key;
    key.type = MELON_TYPE_STRING;
    key.pack.obj = melNewString(vm, "Camera", strlen("Camera"));
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