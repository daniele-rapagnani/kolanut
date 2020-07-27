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

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

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

static TByte load(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, assetDir, MELON_TYPE_STRING, 0);
    melM_arg(vm, mapFile, MELON_TYPE_STRING, 1);

    std::shared_ptr<graphics::Tilemap> tm = 
        ffi::getInstance<graphics::Tilemap>(vm, thisObj->pack.obj)
    ;

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

static TByte draw(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<graphics::Tilemap> tm = 
        ffi::getInstance<graphics::Tilemap>(vm, thisObj->pack.obj)
    ;

    tm->draw();

    return 0;
}

static TByte getLayersCount(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<graphics::Tilemap> tm = 
        ffi::getInstance<graphics::Tilemap>(vm, thisObj->pack.obj)
    ;

    return ffi::push(vm, tm->getLayersCount());
}

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