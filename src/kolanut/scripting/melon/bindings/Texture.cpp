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

static TByte draw(VM* vm)
{
    melM_this(vm, thisObj);
    melM_argOptional(vm, transformVal, MELON_TYPE_OBJECT, 0);
    melM_argOptional(vm, rectVal, MELON_TYPE_OBJECT, 1);
    melM_argOptional(vm, colorVal, MELON_TYPE_OBJECT, 2);

    Recti rect = {};
    Colori color = { 255, 255, 255, 255 };

    std::shared_ptr<Transform3D> transform = 
        ffi::getInstance<Transform3D>(vm, transformVal->pack.obj)
    ;

    ffi::convert(vm, rect, rectVal);
    ffi::convert(vm, color, colorVal);

    std::shared_ptr<graphics::Texture> tex = 
        ffi::getInstance<graphics::Texture>(vm, thisObj->pack.obj)
    ;

    di::get<graphics::Renderer>()->draw(
        tex, 
        *transform.get(),
        rect,
        color
    );

    return 0;
}

static TByte getSize(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<graphics::Texture> tex = 
        ffi::getInstance<graphics::Texture>(vm, thisObj->pack.obj)
    ;

    return ffi::push(vm, tex->getSize());
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "draw", 7, 0, &draw, 1 },
    { "getSize", 1, 0, &getSize, 1 },
    { NULL, 0, 0, NULL }
};

}

void registerTextureBindings(VM* vm)
{
    ffi::newSubmodule(vm, "Kolanut", "Texture", funcs);
}

} // namespace binding
} // namespace melon
} // namespace kola