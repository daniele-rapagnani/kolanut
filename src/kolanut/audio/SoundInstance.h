#pragma once

#include "kolanut/audio/Sound.h"

#include <cstdint>
#include <string>
#include <algorithm>

namespace kola {
namespace audio {

class SoundInstance
{
public:
    void setGain(float gain)
    { 
        this->gain = std::min(std::max(gain, 0.0f), 1.0f); 
        updateGains();
    }

    float getGain() const
    { return this->gain; }

    void setPan(float pan)
    { 
        this->pan = std::min(std::max(pan, -1.0f), 1.0f); 
        updateGains();
    }

    float getPan() const
    { return this->pan; }

    float isFinished() const
    { return this->finished; }

    std::shared_ptr<Sound> getSound() const
    { return this->sound; }

    virtual void restart() = 0;

    void stop()
    { setFinished(true); }

    bool isPaused() const
    { return this->paused; }

    void setPaused(bool paused)
    { this->paused = paused; }

    bool isPlaying() const
    { return !this->isFinished() && !this->isPaused(); }

    bool isLooping() const
    { return this->looping; }

    void setLooping(bool looping)
    { this->looping = looping; }

    virtual void updateGains() = 0;

protected:
    void setSound(std::shared_ptr<Sound> sound)
    { this->sound = sound; }

    void setFinished(bool finished)
    { this->finished = finished; }

private:
    float gain = 1.0;
    float pan = 0.0;

    bool looping = false;

    std::shared_ptr<Sound> sound = {};
    bool finished = true;
    bool paused = false;
};

} // namespace audio
} // namespace kola