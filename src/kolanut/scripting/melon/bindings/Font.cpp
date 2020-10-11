#include "kolanut/core/Logging.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/graphics/Font.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/scripting/melon/bindings/Font.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/Modules.h"
#include "kolanut/scripting/melon/ffi/PopVectors.h"
#include "kolanut/scripting/melon/ffi/PushVectors.h"

#include <cassert>

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

static TByte draw(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, text, MELON_TYPE_STRING, 0);
    melM_argOptional(vm, positionVal, MELON_TYPE_OBJECT, 1);
    melM_argOptional(vm, scaleVal, MELON_TYPE_OBJECT, 2);
    melM_argOptional(vm, colorVal, MELON_TYPE_OBJECT, 3);

    Vec2f position = {};
    Vec2f scale = {1.0f, 1.0f};
    Colori color = { };

    kola::melon::ffi::convert(vm, position, positionVal);
    kola::melon::ffi::convert(vm, scale, scaleVal);
    kola::melon::ffi::convert(vm, color, colorVal);

    std::shared_ptr<graphics::Font> font = 
        ffi::getInstance<graphics::Font>(vm, thisObj->pack.obj)
    ;

    di::get<graphics::Renderer>()->draw(
        melM_strFromObj(text->pack.obj)->string,
        melM_strFromObj(text->pack.obj)->len,
        font,
        position, 
        scale,
        color
    );

    return 0;
}

static TByte getTextSize(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, text, MELON_TYPE_STRING, 0);

    std::shared_ptr<graphics::Font> font = 
        ffi::getInstance<graphics::Font>(vm, thisObj->pack.obj)
    ;

    String* textStr = melM_strFromObj(text->pack.obj);
    Sizei size = font->getTextSize(textStr->string, textStr->len);

    ffi::push(vm, size);

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "draw", 5, 0, &draw, 1 },
    { "getTextSize", 1, 0, &getTextSize, 1 },
    { NULL, 0, 0, NULL }
};

}

void registerFontBindings(VM* vm)
{
    ffi::newSubmodule(vm, "Kolanut", "Font", funcs);
}

} // namespace binding
} // namespace melon
} // namespace kola