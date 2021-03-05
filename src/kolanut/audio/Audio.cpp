
#include "kolanut/core/Logging.h"
#include "kolanut/audio/Audio.h"
#include "kolanut/audio/miniaudio/MiniaudioAudioEngine.h"

#include <cassert>

namespace kola {
namespace audio {

std::shared_ptr<AudioEngine> createAudioEngine(const Config& conf)
{
    switch(conf.engine)
    {
        case Engine::MINIAUDIO:
            return std::make_shared<MiniaudioAudioEngine>();
        
        default:
            assert(false);
            knM_logFatal("Unknown renderer: " << static_cast<int>(conf.engine));
    }

    return nullptr;
}

} // namespace audio
} // namespace kola