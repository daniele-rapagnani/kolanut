#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/audio/AudioEngine.h"
#include "kolanut/audio/Sound.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/Modules.h"
#include "kolanut/scripting/melon/ffi/PopVectors.h"
#include "kolanut/scripting/melon/ffi/PushVectors.h"

#include <cassert>

/***
 * @module
 * 
 * This class represents a sound sample.
 */

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

/***
 * Play this sound.
 * 
 * @arg ?gain The gain to use to play the sound, defaults to 1.0
 * @arg ?pan The pan to use to play the sound: 0.0 is dead center, -1.0 left and 1.0 right
 * 
 * @returns An instance of `SoundInstance` that can be used to interact with the played sound.
 */

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

    std::shared_ptr<audio::SoundInstance> si = 
        di::get<audio::AudioEngine>()->playSound(sound, gain, pan)
    ;

    kola::melon::ffi::pushInstance(vm, "Kolanut", "SoundInstance", si);

    return 1;
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