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

static TByte setPosition(VM* vm)
{
    melM_arg(vm, posVal, MELON_TYPE_OBJECT, 0);

    Vec2f pos;
    ffi::convert(vm, pos, posVal);

    di::get<graphics::Renderer>()->setCameraPosition(pos);
    return 0;
}

static TByte getPosition(VM* vm)
{
    Vec2f cPos = di::get<graphics::Renderer>()->getCameraPosition();
    return ffi::push(vm, cPos);
}

static TByte setZoom(VM* vm)
{
    melM_arg(vm, zoomVal, MELON_TYPE_NUMBER, 0);
    di::get<graphics::Renderer>()->setCameraZoom(zoomVal->pack.value.number);
    return 0;
}

static TByte getZoom(VM* vm)
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
    ffi::newSubmodule(vm, "Kolanut", "Camera", funcs);
}

} // namespace binding
} // namespace melon
} // namespace kola