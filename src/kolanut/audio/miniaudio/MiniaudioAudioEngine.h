#pragma once

#include "kolanut/audio/AudioEngine.h"
#include "kolanut/audio/Sound.h"
#include "kolanut/audio/miniaudio/MiniaudioSound.h"

#include <miniaudio.h>

#include <memory>
#include <vector>

namespace kola {
namespace audio {

class MiniaudioAudioEngine : public AudioEngine
{
public:
    bool init(const Config& config) override;
    std::shared_ptr<Sound> loadSound(const std::string& fileName) override;
    void playSound(std::shared_ptr<Sound> sample) override;

    const std::vector<std::shared_ptr<MiniaudioSound>>& getPlayingSounds()
    { return this->playing; }

private:
    ma_device device = {};
    std::vector<std::shared_ptr<MiniaudioSound>> playing = {};
};

} // namespace audio
} // namespace kola