#include "kolanut/scripting/melon/modules/KolanutModule.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/PopVectors.h"
#include "kolanut/scripting/melon/ffi/PushVectors.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/events/EventSystem.h"
#include "kolanut/audio/AudioEngine.h"
#include "kolanut/audio/Sound.h"
#include "kolanut/core/DIContainer.h"

extern "C" {
#include <melon/core/tstring.h>
#include <melon/modules/modules.h>
}

extern "C" {

static TByte onLoad(VM* vm)
{
    return 0;
}

static TByte onUpdate(VM* vm)
{
    return 0;
}

static TByte onDraw(VM* vm)
{
    return 0;
}

static TByte onDrawUI(VM* vm)
{
    return 0;
}

static TByte onQuit(VM* vm)
{
    melM_stackEnsure(&vm->stack, vm->stack.top + 1);
    Value* res = melM_stackAllocRaw(&vm->stack);
    res->type = MELON_TYPE_BOOL;
    res->pack.value.boolean = true;

    return 1;
}

static TByte onKeyPressed(VM* vm)
{
    return 0;
}

static TByte loadSprite(VM* vm)
{
    melM_arg(vm, file, MELON_TYPE_STRING, 0);

    std::shared_ptr<kola::graphics::Renderer> renderer = 
        kola::di::get<kola::graphics::Renderer>()
    ;

    std::shared_ptr<kola::graphics::Texture> texture = 
        renderer->loadTexture(melM_strDataFromObj(file->pack.obj))
    ;

    if (!texture)
    {
        melM_vstackPushNull(&vm->stack);
        return 1;
    }

    kola::melon::ffi::pushInstance(vm, "Kolanut", "Texture", texture);
    return 1;
}

static TByte loadFont(VM* vm)
{
    melM_arg(vm, file, MELON_TYPE_STRING, 0);
    melM_argOptional(vm, size, MELON_TYPE_INTEGER, 1);

    std::shared_ptr<kola::graphics::Renderer> renderer = 
        kola::di::get<kola::graphics::Renderer>()
    ;

    size_t sizeInt = kola::graphics::Font::DEF_SIZE;

    if (size->type == MELON_TYPE_INTEGER)
    {
        sizeInt = size->pack.value.integer;
    }

    std::shared_ptr<kola::graphics::Font> font = 
        renderer->loadFont(melM_strDataFromObj(file->pack.obj), sizeInt)
    ;

    if (!font)
    {
        melM_vstackPushNull(&vm->stack);
        return 1;
    }

    kola::melon::ffi::pushInstance(vm, "Kolanut", "Font", font);
    return 1;
}

static TByte loadSound(VM* vm)
{
    melM_arg(vm, file, MELON_TYPE_STRING, 0);

    std::shared_ptr<kola::audio::AudioEngine> audio = 
        kola::di::get<kola::audio::AudioEngine>()
    ;

    std::shared_ptr<kola::audio::Sound> sound = 
        audio->loadSound(melM_strDataFromObj(file->pack.obj))
    ;

    kola::melon::ffi::pushInstance(vm, "Kolanut", "Sound", sound);
    return 1;
}

static TByte getScreenSize(VM* vm)
{
    std::shared_ptr<kola::graphics::Renderer> renderer = 
        kola::di::get<kola::graphics::Renderer>()
    ;

    kola::melon::ffi::push(vm, renderer->getResolution());

    return 1;
}

static TByte getDesignResolution(VM* vm)
{
    std::shared_ptr<kola::graphics::Renderer> renderer = 
        kola::di::get<kola::graphics::Renderer>()
    ;

    kola::melon::ffi::push(vm, renderer->getDesignResolution());

    return 1;
}

static TByte getTime(VM* vm)
{
    std::shared_ptr<kola::events::EventSystem> eventSystem = 
        kola::di::get<kola::events::EventSystem>()
    ;

    kola::melon::ffi::push(vm, eventSystem->getTimeMS());

    return 1;
}

static TByte drawRect(VM* vm)
{
    melM_arg(vm, rectVal, MELON_TYPE_OBJECT, 0);
    melM_argOptional(vm, colorVal, MELON_TYPE_OBJECT, 1);

    kola::Rectf rect = {};
    kola::Colori color = { };

    std::shared_ptr<kola::graphics::Renderer> renderer = 
        kola::di::get<kola::graphics::Renderer>()
    ;

    kola::melon::ffi::convert(vm, rect, rectVal);
    kola::melon::ffi::convert(vm, color, colorVal);

    renderer->draw(rect, color);

    return 0;
}

static TByte drawLine(VM* vm)
{
    melM_arg(vm, aVal, MELON_TYPE_OBJECT, 0);
    melM_arg(vm, bVal, MELON_TYPE_OBJECT, 1);
    melM_argOptional(vm, colorVal, MELON_TYPE_OBJECT, 2);

    kola::Vec2f a = {};
    kola::Vec2f b = {};
    kola::Colori color = { };

    std::shared_ptr<kola::graphics::Renderer> renderer = 
        kola::di::get<kola::graphics::Renderer>()
    ;

    kola::melon::ffi::convert(vm, a, aVal);
    kola::melon::ffi::convert(vm, b, bVal);
    kola::melon::ffi::convert(vm, color, colorVal);

    renderer->draw(a, b, color);

    return 0;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "onLoad", 0, 0, &onLoad },
    { "onUpdate", 0, 0, &onUpdate },
    { "onDraw", 0, 0, &onDraw },
    { "onDrawUI", 0, 0, &onDraw },
    { "onQuit", 0, 0, &onQuit },
    { "onKeyPressed", 2, 0, &onQuit },
    
    { "loadSprite", 1, 0, &loadSprite },
    { "loadFont", 2, 0, &loadFont },
    { "loadSound", 1, 0, &loadSound },
    { "getScreenSize", 0, 0, &getScreenSize },
    { "getDesignResolution", 0, 0, &getDesignResolution },

    { "getTime", 0, 0, &getTime },

    { "drawRect", 2, 0, &drawRect },
    { "drawLine", 3, 0, &drawLine },

    { NULL, 0, 0, NULL }
};

TRet kolanutModuleInit(VM* vm)
{
    return melNewModule(vm, funcs);
}

} // extern "C"