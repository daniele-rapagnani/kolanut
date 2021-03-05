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

    std::shared_ptr<audio::Sound> sound = 
        ffi::getInstance<audio::Sound>(vm, thisObj->pack.obj)
    ;

    di::get<audio::AudioEngine>()->playSound(sound);

    return 0;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "play", 1, 0, &play, 1 },
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