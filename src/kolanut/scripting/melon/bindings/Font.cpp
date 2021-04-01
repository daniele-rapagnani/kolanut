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

/***
 * @module
 * 
 * This class represents an instance of a font.
 */

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

/***
 * Draws some text using this font.
 * 
 * @arg text A string with the text to draw
 * @arg ?position A `Vector2` with the position where the text should be drawn
 * @arg ?size The size of the text in pixels, defaults to the size used when loading the font
 * @arg ?color The `Color` to use to draw
 */

static TByte draw(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, text, MELON_TYPE_STRING, 0);
    melM_argOptional(vm, positionVal, MELON_TYPE_OBJECT, 1);
    melM_argOptional(vm, sizeVal, MELON_TYPE_NUMBER, 2);
    melM_argOptional(vm, colorVal, MELON_TYPE_OBJECT, 3);
    
    std::shared_ptr<graphics::Font> font = 
        ffi::getInstance<graphics::Font>(vm, thisObj->pack.obj)
    ;

    Vec2f position = {};
    float size = static_cast<float>(font->getNativeFontSize());
    Colori color = { };

    kola::melon::ffi::convert(vm, position, positionVal);
    kola::melon::ffi::convert(vm, size, sizeVal);
    kola::melon::ffi::convert(vm, color, colorVal);

    Vec2f scale = {
        size / font->getNativeFontSize(),
        size / font->getNativeFontSize()
    };

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

/***
 * Returns the size of some text rendered with this font at its native size.
 * 
 * @arg text The text of which you want to know the size when drawn
 * 
 * @returns A `Vector2` with the text size
 */

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

/***
 * Gets the size at which this font has been rasterized, the same used
 * to load the font.
 * 
 * @returns The size used to rasterize this font in pixels
 */

static TByte getNativeSize(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<graphics::Font> font = 
        ffi::getInstance<graphics::Font>(vm, thisObj->pack.obj)
    ;

    ffi::push(vm, font->getNativeFontSize());

    return 1;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "draw", 5, 0, &draw, 1 },
    { "getTextSize", 1, 0, &getTextSize, 1 },
    { "getNativeSize", 1, 0, &getNativeSize, 1 },
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