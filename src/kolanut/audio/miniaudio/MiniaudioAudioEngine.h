#pragma once

#include "kolanut/audio/AudioEngine.h"
#include "kolanut/audio/Sound.h"
#include "kolanut/audio/SoundInstance.h"
#include "kolanut/audio/miniaudio/MiniaudioSound.h"

#include <miniaudio.h>

#include <memory>
#include <vector>
#include <list>
#include <mutex>

namespace kola {
namespace audio {

class MiniaudioAudioEngine : public AudioEngine
{
public:
    static constexpr uint8_t NUM_CHANNELS = 2;
    static constexpr uint32_t SAMPLE_RATE = 44100;

public:
    ~MiniaudioAudioEngine();

    bool init(const Config& config) override;
    std::shared_ptr<Sound> loadSound(const std::string& fileName) override;
    
    std::shared_ptr<SoundInstance> playSound(
        std::shared_ptr<Sound> sample, 
        float gain = 1.0f, 
        float pan = 0.0f
    ) override;

    const std::list<std::shared_ptr<SoundInstance>>& getPlayingSounds()
    { return this->playing; }

    std::mutex& getPlayingMutex()
    { return this->playingMutex; }

private:
    ma_device device = {};
    std::list<std::shared_ptr<SoundInstance>> playing = {};
    std::mutex playingMutex = {};
};

} // namespace audio
} // namespace kola