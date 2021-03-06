#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/audio/AudioEngine.h"
#include "kolanut/audio/Sound.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/Modules.h"
#include "kolanut/scripting/melon/ffi/PopVectors.h"
#include "kolanut/scripting/melon/ffi/PushVectors.h"

#include <cassert>

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

static TByte play(VM* vm)
{
    melM_this(vm, thisObj);
    melM_argOptional(vm, gainVal, MELON_TYPE_NUMBER, 0);
    melM_argOptional(vm, panVal, MELON_TYPE_NUMBER, 1);

    float gain = 1.0f;
    float pan = 0.0f;

    kola::melon::ffi::convert(vm, gain, gainVal);
    kola::melon::ffi::convert(vm, pan, panVal);

    std::shared_ptr<audio::Sound> sound = 
        ffi::getInstance<audio::Sound>(vm, thisObj->pack.obj)
    ;

    di::get<audio::AudioEngine>()->playSound(sound, gain, pan);

    return 0;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "play", 3, 0, &play, 1 },
    { NULL, 0, 0, NULL }
};

}

void registerSoundBindings(VM* vm)
{
    ffi::newSubmodule(vm, "Kolanut", "Sound", funcs);
}

} // namespace binding
} // namespace melon
} // namespace kola