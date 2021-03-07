#pragma once

#include "kolanut/audio/miniaudio/MiniaudioAudioEngine.h"
#include "kolanut/audio/SoundInstance.h"

#include <miniaudio.h>

#include <cstdint>
#include <vector>
#include <vector>
#include <array>

namespace kola {
namespace audio {

class MiniaudioAudioEngine;

class MiniaudioSoundInstance : public SoundInstance
{
    friend class MiniaudioAudioEngine;

public:
    void updateGains() override;

    void restart() override;

    ma_decoder* getDecoder()
    { return &this->decoder; }

    const std::array<float, MiniaudioAudioEngine::NUM_CHANNELS>& getGains() const
    { return this->gains; }

    void setRestart(bool restart)
    { this->restartFlag = restart; }

    bool shouldRestart() const
    { return this->restartFlag; }

private:
    ma_decoder decoder = {};
    bool restartFlag = false;
    std::array<float, MiniaudioAudioEngine::NUM_CHANNELS> gains = {};
};

} // namespace audio
} // namespace kola