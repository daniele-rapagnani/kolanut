#include "kolanut/scripting/melon/bindings/Tilemap.h"
#include "kolanut/core/Logging.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/graphics/Texture.h"
#include "kolanut/graphics/Tilemap.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/Modules.h"
#include "kolanut/scripting/melon/ffi/PopVectors.h"
#include "kolanut/scripting/melon/ffi/PushVectors.h"
#include "kolanut/scripting/melon/ffi/PushContainers.h"
#include "kolanut/scripting/melon/bindings/TilemapFFI.h"

#include <cassert>

/***
 * @module
 * 
 * This class represents a Tiled tilemap
 */

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

/***
 * Creates a new `Tilemap` instance
 *
 * @returns A new `Tilemap` object
 */

static TByte create(VM* vm)
{
    std::shared_ptr<graphics::Tilemap> tilemap = 
        std::make_shared<graphics::Tilemap>()
    ;

    if (!ffi::pushInstance(vm, "Kolanut", "Tilemap", tilemap))
    {
        knM_logError("Can't push native instance of tilemap");
        return 0;
    }

    return 1;
}

/***
 * Loads a tilemap from file into this instance.
 * 
 * @arg assetDir The directory in which the tilemap is located
 * @arg mapFile The filename of the tilemap to load
 * 
 * @returns `true` on success, `false` otherwise
 */

static TByte load(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, assetDir, MELON_TYPE_STRING, 0);
    melM_arg(vm, mapFile, MELON_TYPE_STRING, 1);

    std::shared_ptr<graphics::Tilemap> tm = 
        ffi::getInstance<graphics::Tilemap>(vm, thisObj->pack.obj)
    ;

    String* str = melM_strFromObj(assetDir->pack.obj);
    const char* ds = melM_strDataFromObj(assetDir->pack.obj);

    std::string dirStr = std::string(
        melM_strDataFromObj(assetDir->pack.obj),
        melM_strFromObj(assetDir->pack.obj)->len
    );

    std::string mapFileStr = std::string(
        melM_strDataFromObj(mapFile->pack.obj), 
        melM_strFromObj(mapFile->pack.obj)->len
    );

    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* newVal = melM_stackAllocRaw(&vm->stack);
    newVal->type = MELON_TYPE_BOOL;
    newVal->pack.value.boolean = tm->load(dirStr, mapFileStr) ? 1 : 0;

    return 1;
}

/***
 * Instantiate a specific layer.
 * Only instantiated layers will be drawn when this tilemap is drawn.
 * 
 * @arg layerNum The number of the layer to instantiate
 * @arg position A `Vector2` with the position where the layer should be istantiated
 * @arg ?scale The scale at which the layer should be instantiated
 */

static TByte instantiateLayer(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, layerNumVal, MELON_TYPE_INTEGER, 0);
    melM_arg(vm, positionVal, MELON_TYPE_OBJECT, 1);
    melM_argOptional(vm, scaleVal, MELON_TYPE_OBJECT, 2);

    std::shared_ptr<graphics::Tilemap> tm = 
        ffi::getInstance<graphics::Tilemap>(vm, thisObj->pack.obj)
    ;

    Vec2f pos;
    ffi::convert(vm, pos, positionVal);

    Vec2f scale = { 1.0f, 1.0f };
    
    if (scaleVal->type == MELON_TYPE_OBJECT)
    {
        ffi::convert(vm, scale, scaleVal);
    }

    tm->instantiateLayer(
        layerNumVal->pack.value.integer,
        pos,
        scale
    );

    return 0;
}

/***
 * Instantiate all layers.
 * Only instantiated layers will be drawn when this tilemap is drawn.
 * 
 * @arg position A `Vector2` with the position where the layers should be istantiated
 * @arg ?scale The scale at which the layers should be instantiated
 */

static TByte instantiate(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, positionVal, MELON_TYPE_OBJECT, 0);
    melM_argOptional(vm, scaleVal, MELON_TYPE_OBJECT, 1);

    std::shared_ptr<graphics::Tilemap> tm = 
        ffi::getInstance<graphics::Tilemap>(vm, thisObj->pack.obj)
    ;

    Vec2f pos;
    ffi::convert(vm, pos, positionVal);

    Vec2f scale = { 1.0f, 1.0f };
    
    if (scaleVal->type == MELON_TYPE_OBJECT)
    {
        ffi::convert(vm, scale, scaleVal);
    }

    tm->instantiate(pos, scale);

    return 0;
}

/***
 * Draw all instantiated layers at their respective positions
 * and scales
 */

static TByte draw(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<graphics::Tilemap> tm = 
        ffi::getInstance<graphics::Tilemap>(vm, thisObj->pack.obj)
    ;

    tm->draw();

    return 0;
}

/***
 * Gets the number of layers in this tilemap.
 * 
 * @returns The number of layers in this tilemap.
 */

static TByte getLayersCount(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<graphics::Tilemap> tm = 
        ffi::getInstance<graphics::Tilemap>(vm, thisObj->pack.obj)
    ;

    return ffi::push(vm, tm->getLayersCount());
}

/***
 * Gets an object in this tilemap.
 * 
 * The resulting object will be an object with the following structure:
 * ```
 * {
 *  name: ...
 *  type: ...
 *  visible: ...
 *  x: ...
 *  y: ...
 *  w: ...
 *  h: ...
 * }
 * ```
 * 
 * @arg group The group of which this object is part of
 * @arg name The name of the object you are looking for
 * 
 * @returns An object or `null`
 */

static TByte getObjectByName(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, group, MELON_TYPE_INTEGER, 0);
    melM_arg(vm, name, MELON_TYPE_STRING, 1);

    std::shared_ptr<graphics::Tilemap> tm = 
        ffi::getInstance<graphics::Tilemap>(vm, thisObj->pack.obj)
    ;

    return ffi::push(vm, tm->getObjectByName(
        group->pack.value.integer,
        melM_strDataFromObj(name->pack.obj)
    ));
}

/***
 * Gets an objects in this tilemap by type.
 * See `getObjectByName` for the result's structure.
 * 
 * @arg group The parent group of the objects you're looking fore
 * @arg type The type of the objects you are looking for
 * 
 * @returns An array of objects
 */

static TByte getObjectsByType(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, group, MELON_TYPE_INTEGER, 0);
    melM_arg(vm, type, MELON_TYPE_STRING, 1);

    std::shared_ptr<graphics::Tilemap> tm = 
        ffi::getInstance<graphics::Tilemap>(vm, thisObj->pack.obj)
    ;

    return ffi::push(vm, tm->getObjectsByType(
        group->pack.value.integer,
        melM_strDataFromObj(type->pack.obj)
    ));
}

/***
 * Gets a tile by position.
 * 
 * If a tile is found an object with the following structure is returned:
 * ```
 * {
 *  gid: ...
 *  id: ...
 *  position: ...
 *  rect: ...
 * }
 * ```
 * 
 * @arg layer The layer on which to check for tiles at the specific position
 * @arg pos A `Vector2` with the position where you want to check for tiles
 * 
 * @returns A an object representing the tile or `null` if no tile is present
 */

static TByte getTileAt(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, layer, MELON_TYPE_INTEGER, 0);
    melM_arg(vm, posVal, MELON_TYPE_OBJECT, 1);

    std::shared_ptr<graphics::Tilemap> tm = 
        ffi::getInstance<graphics::Tilemap>(vm, thisObj->pack.obj)
    ;

    Vec2f pos;
    ffi::convert(vm, pos, posVal);

    return ffi::push(vm, tm->getTileAt(
        layer->pack.value.integer, 
        pos
    ));
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "create", 0, 0, &create, 0 },
    { "load", 3, 0, &load, 1 },
    { "instantiateLayer", 4, 0, &instantiateLayer, 1 },
    { "instantiate", 3, 0, &instantiate, 1 },
    { "draw", 1, 0, &draw, 1 },
    { "getLayersCount", 1, 0, &getLayersCount, 1 },
    { "getObjectByName", 3, 0, &getObjectByName, 1 },
    { "getObjectsByType", 3, 0, &getObjectsByType, 1 },
    { "getTileAt", 3, 0, &getTileAt, 1 },
    { NULL, 0, 0, NULL }
};

}

void registerTilemapBindings(VM* vm)
{
    ffi::newSubmodule(vm, "Kolanut", "Tilemap", funcs);
}

} // namespace binding
} // namespace melon
} // namespace kola