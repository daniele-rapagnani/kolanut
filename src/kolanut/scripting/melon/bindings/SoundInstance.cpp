#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/audio/AudioEngine.h"
#include "kolanut/audio/Sound.h"
#include "kolanut/audio/SoundInstance.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/Modules.h"
#include "kolanut/scripting/melon/ffi/PopVectors.h"
#include "kolanut/scripting/melon/ffi/PushVectors.h"

#include <cassert>

/***
 * @module
 * 
 * This class represents a sound that's playing or has been played.
 * It can be used to interact with a sound while it's playing.
 */

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

/***
 * Gets the current gain
 * 
 * @returns the current gain
 */

static TByte getGain(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    ffi::push(vm, si->getGain());
    return 1;
}

/***
 * Gets the current pan
 * 
 * @returns the current pan: 0.0 is center, -1.0 is left, 1.0 is right
 */

static TByte getPan(VM* vm)
{
    melM_this(vm, thisObj);
    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    ffi::push(vm, si->getPan());
    return 1;
}

/***
 * Sets the current gain
 * 
 * @arg gain The gain value, 1.0 is the original sample's volume
 */

static TByte setGain(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, gainVal, MELON_TYPE_NUMBER, 0);

    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    float gain = 1.0f;
    ffi::convert(vm, gain, gainVal);

    si->setGain(gain);
    return 0;
}

/***
 * Sets the current pan
 * 
 * @arg pan The pan value, see `getPan`
 */

static TByte setPan(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, panVal, MELON_TYPE_NUMBER, 0);

    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    float pan = 0.0f;
    ffi::convert(vm, pan, panVal);

    si->setPan(pan);

    return 0;
}

/***
 * Checks if this sound is still playing
 * 
 * @returns `true` if it's playing `false` otherwise
 */

static TByte isPlaying(VM* vm)
{
    melM_this(vm, thisObj);
    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    ffi::push(vm, si->isPlaying());
    
    return 1;
}

/***
 * Checks if this sound is paused.
 * If a sound has been paused it is still playing as it can be resumed.
 * 
 * @returns `true` if it's paused `false` otherwise
 */

static TByte isPaused(VM* vm)
{
    melM_this(vm, thisObj);
    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    ffi::push(vm, si->isPaused());
    
    return 1;
}

/***
 * Pause or resume this sound
 * 
 * @arg paused `true` for pausing, `false` for resuming
 */

static TByte setPaused(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, pausedVal, MELON_TYPE_BOOL, 0);

    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    bool paused = false;
    ffi::convert(vm, paused, pausedVal);

    si->setPaused(paused);

    return 0;
}

/***
 * Checks if this sound's looping is enabled.
 * 
 * @returns `true` if it's looping `false` otherwise
 */

static TByte isLooping(VM* vm)
{
    melM_this(vm, thisObj);
    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    ffi::push(vm, si->isLooping());
    
    return 1;
}

/***
 * Enable/disables looping for this sound
 * 
 * @arg paused `true` for looping, `false` otherwise
 */

static TByte setLooping(VM* vm)
{
    melM_this(vm, thisObj);
    melM_arg(vm, loopingVal, MELON_TYPE_BOOL, 0);

    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    bool looping = false;
    ffi::convert(vm, looping, loopingVal);

    si->setLooping(looping);

    return 0;
}

/***
 * Stop the sound. Once stopped the sound's instance will be released
 * and you will not be able to play this sound anymore.
 */

static TByte stop(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    si->stop();
    
    return 0;
}

/***
 * Restarts playing this sound from the beginning
 */

static TByte restart(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    si->restart();
    
    return 0;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "getGain", 1, 0, &getGain, 1 },
    { "getPan", 1, 0, &getPan, 1 },
    { "setGain", 2, 0, &setGain, 1 },
    { "setPan", 2, 0, &setPan, 1 },
    { "isPlaying", 1, 0, &isPlaying, 1 },
    { "isPaused", 1, 0, &isPaused, 1 },
    { "setPaused", 2, 0, &setPaused, 1 },
    { "isLooping", 1, 0, &isLooping, 1 },
    { "setLooping", 2, 0, &setLooping, 1 },
    { "stop", 1, 0, &stop, 1 },
    { "restart", 1, 0, &restart, 1 },
    { NULL, 0, 0, NULL }
};

}

void registerSoundInstanceBindings(VM* vm)
{
    ffi::newSubmodule(vm, "Kolanut", "SoundInstance", funcs);
}

} // namespace binding
} // namespace melon
} // namespace kola