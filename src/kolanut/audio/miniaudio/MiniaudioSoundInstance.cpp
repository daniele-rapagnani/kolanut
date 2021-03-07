#include "kolanut/audio/miniaudio/MiniaudioSoundInstance.h"

#include <cassert>

namespace kola {
namespace audio {

void MiniaudioSoundInstance::updateGains()
{
    assert(this->gains.size() == 2);

    // Back to 0 hard left, 1 hard right
    float pan = (getPan() + 1.0f) / 2.0f;

    this->gains[0] = std::max(
        std::numeric_limits<float>::epsilon(),
        (getGain() * (1.0f - pan))
    );

    this->gains[1] = std::max(
        std::numeric_limits<float>::epsilon(),
        (getGain() * pan)
    );  
}

void MiniaudioSoundInstance::restart()
{
    setRestart(true);
}

} // namespace audio
} // namespace kola