#pragma once

#include "kolanut/audio/Sound.h"

#include <miniaudio.h>

#include <cstdint>
#include <vector>

namespace kola {
namespace audio {

class MiniaudioSound : public Sound
{
public:
    ma_decoder decoder;
    std::vector<char> data;
};

} // namespace audio
} // namespace kola