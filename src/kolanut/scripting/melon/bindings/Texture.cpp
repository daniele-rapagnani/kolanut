#include "kolanut/core/Logging.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/graphics/Texture.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/scripting/melon/bindings/Texture.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/Modules.h"
#include "kolanut/scripting/melon/ffi/PopVectors.h"
#include "kolanut/scripting/melon/ffi/PushVectors.h"

#include <cassert>

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

TByte draw(VM* vm)
{
    melM_this(vm, thisObj);
    melM_argOptional(vm, positionVal, MELON_TYPE_OBJECT, 0);
    melM_argOptional(vm, angle, MELON_TYPE_NUMBER, 1);
    melM_argOptional(vm, scaleVal, MELON_TYPE_OBJECT, 2);
    melM_argOptional(vm, originVal, MELON_TYPE_OBJECT, 3);

    Vec2f position = {};
    Vec2f scale = {};
    Vec2f origin = {};

    kola::melon::ffi::convert(vm, position, positionVal);
    kola::melon::ffi::convert(vm, scale, scaleVal);
    kola::melon::ffi::convert(vm, origin, originVal);

    std::shared_ptr<graphics::Texture> tex = 
        ffi::getInstance<graphics::Texture>(vm, thisObj->pack.obj)
    ;

    di::get<graphics::Renderer>()->draw(
        tex, 
        position, 
        angle->pack.value.number, 
        scale,
        origin
    );

    return 0;
}

TByte drawRect(VM* vm)
{
    melM_this(vm, thisObj);
    melM_argOptional(vm, positionVal, MELON_TYPE_OBJECT, 0);
    melM_argOptional(vm, angle, MELON_TYPE_NUMBER, 1);
    melM_argOptional(vm, scaleVal, MELON_TYPE_OBJECT, 2);
    melM_argOptional(vm, originVal, MELON_TYPE_OBJECT, 3);
    melM_argOptional(vm, rectVal, MELON_TYPE_OBJECT, 4);

    Vec2f position = {};
    Vec2f scale = {};
    Vec2f origin = {};
    Vec4i rect = {};

    kola::melon::ffi::convert(vm, position, positionVal);
    kola::melon::ffi::convert(vm, scale, scaleVal);
    kola::melon::ffi::convert(vm, origin, originVal);
    kola::melon::ffi::convert(vm, rect, rectVal);

    std::shared_ptr<graphics::Texture> tex = 
        ffi::getInstance<graphics::Texture>(vm, thisObj->pack.obj)
    ;

    di::get<graphics::Renderer>()->draw(
        tex, 
        position, 
        angle->pack.value.number, 
        scale,
        origin,
        rect
    );

    return 0;
}

TByte getSize(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<graphics::Texture> tex = 
        ffi::getInstance<graphics::Texture>(vm, thisObj->pack.obj)
    ;

    return ffi::push(vm, tex->getSize());
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "draw", 5, 0, &draw, 1 },
    { "drawRect", 6, 0, &drawRect, 1 },
    { "getSize", 1, 0, &getSize, 1 },
    { NULL, 0, 0, NULL }
};

}

void registerTextureBindings(VM* vm)
{
    Value* val = ffi::getModule(*vm, "Kolanut");
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