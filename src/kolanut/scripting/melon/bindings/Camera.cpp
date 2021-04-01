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

/***
 * @module
 * 
 * This module can be used to interact with the game's camera.
 */

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

/***
 * Sets the position of the camera.
 * 
 * @arg pos A `Vector2` with the new position
 */

static TByte setPosition(VM* vm)
{
    melM_arg(vm, posVal, MELON_TYPE_OBJECT, 0);

    Vec2f pos;
    ffi::convert(vm, pos, posVal);

    di::get<graphics::Renderer>()->setCameraPosition(pos);
    return 0;
}

/***
 * Gets the position of the camera.
 * 
 * @returns A `Vector2` with the current position
 */

static TByte getPosition(VM* vm)
{
    Vec2f cPos = di::get<graphics::Renderer>()->getCameraPosition();
    return ffi::push(vm, cPos);
}

/***
 * Sets the camera's zoom.
 * 
 * @arg zoom A value of 1.0 corresponds to the design resolution, 2.0 to twice the design resolution.
 */

static TByte setZoom(VM* vm)
{
    melM_arg(vm, zoomVal, MELON_TYPE_NUMBER, 0);
    di::get<graphics::Renderer>()->setCameraZoom(zoomVal->pack.value.number);
    return 0;
}

/***
 * Gets the camera's zoom.
 * 
 * @returns The current camera zoom
 */

static TByte getZoom(VM* vm)
{
    float zoom = di::get<graphics::Renderer>()->getCameraZoom();
    return ffi::push(vm, zoom);
}

/***
 * Sets the camera's origin.
 * 
 * @arg origin A `Vector2` with the new camera's origin
 */

static TByte setOrigin(VM* vm)
{
    melM_arg(vm, origVal, MELON_TYPE_OBJECT, 0);

    Vec2f orig;
    ffi::convert(vm, orig, origVal);

    di::get<graphics::Renderer>()->setCameraOrigin(orig);
    return 0;
}

/***
 * Gets the camera's origin.
 * 
 * @returns A `Vector2` with the current camera's origin
 */

static TByte getOrigin(VM* vm)
{
    Vec2f cOrig = di::get<graphics::Renderer>()->getCameraOrigin();
    return ffi::push(vm, cOrig);
}

/***
 * Sets position, zoom and origin with one call.
 * This is more efficient than calling them individually because
 * the internal camera matrix is calculated only once.
 * 
 * @arg pos A `Vector2` with the new camera position
 * @arg zoom The new zoom, see `setZoom`
 * @arg origin A `Vector2` with the new camera's origin
 */

static TByte set(VM* vm)
{
    melM_arg(vm, posVal, MELON_TYPE_OBJECT, 0);
    melM_arg(vm, zoomVal, MELON_TYPE_NUMBER, 1);
    melM_arg(vm, origVal, MELON_TYPE_OBJECT, 2);

    Vec2f pos;
    ffi::convert(vm, pos, posVal);

    Vec2f orig;
    ffi::convert(vm, orig, origVal);

    float zoom;
    ffi::convert(vm, zoom, zoomVal);

    di::get<graphics::Renderer>()->setCamera(pos, orig, zoom);

    return 0;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "setPosition", 1, 0, &setPosition, 0 },
    { "getPosition", 0, 0, &getPosition, 0 },
    { "setZoom", 1, 0, &setZoom, 0 },
    { "getZoom", 0, 0, &getZoom, 0 },
    { "setOrigin", 1, 0, &setOrigin, 0 },
    { "getOrigin", 0, 0, &getOrigin, 0 },
    { "set", 3, 0, &set, 0 },
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