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

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

static TByte getGain(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    ffi::push(vm, si->getGain());
    return 1;
}

static TByte getPan(VM* vm)
{
    melM_this(vm, thisObj);
    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    ffi::push(vm, si->getPan());
    return 1;
}

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

static TByte isPlaying(VM* vm)
{
    melM_this(vm, thisObj);
    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    ffi::push(vm, si->isPlaying());
    
    return 1;
}

static TByte isPaused(VM* vm)
{
    melM_this(vm, thisObj);
    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    ffi::push(vm, si->isPaused());
    
    return 1;
}

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

static TByte isLooping(VM* vm)
{
    melM_this(vm, thisObj);
    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    ffi::push(vm, si->isLooping());
    
    return 1;
}

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

static TByte stop(VM* vm)
{
    melM_this(vm, thisObj);

    std::shared_ptr<audio::SoundInstance> si = 
        ffi::getInstance<audio::SoundInstance>(vm, thisObj->pack.obj)
    ;

    si->stop();
    
    return 0;
}

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