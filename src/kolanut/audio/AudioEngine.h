#pragma once

#include "kolanut/audio/Audio.h"
#include "kolanut/audio/Sound.h"
#include "kolanut/audio/SoundInstance.h"

#include <cstdint>
#include <string>
#include <memory>

namespace kola {
namespace audio {

class AudioEngine
{
public:
    virtual bool init(const Config& config) = 0;
    virtual std::shared_ptr<Sound> loadSound(const std::string& fileName) = 0;

    virtual std::shared_ptr<SoundInstance> playSound(
        std::shared_ptr<Sound> sample, 
        float gain = 1.0f, 
        float pan = 0.0f
    ) = 0;
};

} // namespace audio
} // namespace kola