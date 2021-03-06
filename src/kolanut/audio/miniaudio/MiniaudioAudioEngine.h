#pragma once

#include "kolanut/audio/AudioEngine.h"
#include "kolanut/audio/Sound.h"
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
    struct PlayingSound
    {
        std::shared_ptr<MiniaudioSound> sound = {};
        int16_t gainDiv[NUM_CHANNELS] = {1};
        ma_decoder decoder = {};
        bool finished = false;

        void setGain(float gain, float pan);
    };

public:
    ~MiniaudioAudioEngine();
    
    bool init(const Config& config) override;
    std::shared_ptr<Sound> loadSound(const std::string& fileName) override;
    void playSound(std::shared_ptr<Sound> sample, float gain = 1.0f, float pan = 0.0f) override;

    const std::list<PlayingSound>& getPlayingSounds()
    { return this->playing; }

    std::mutex& getPlayingMutex()
    { return this->playingMutex; }

private:
    ma_device device = {};
    std::list<PlayingSound> playing = {};
    std::mutex playingMutex = {};
};

} // namespace audio
} // namespace kola