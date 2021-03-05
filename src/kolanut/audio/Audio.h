#pragma once

#include "kolanut/core/Types.h"

#include <memory>
#include <string>

namespace kola {
namespace audio {

class AudioEngine;

enum class Engine
{
    NONE = 0,
    MINIAUDIO
};

struct Config
{
    Engine engine = Engine::MINIAUDIO;
};

std::shared_ptr<AudioEngine> createAudioEngine(const Config& conf);

} // namespace audio
} // namespace kola