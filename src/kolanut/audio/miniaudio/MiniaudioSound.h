#pragma once

#include "kolanut/audio/Sound.h"

#include <cstdint>
#include <vector>
#include <string>

namespace kola {
namespace audio {

class MiniaudioSound : public Sound
{
public:
    std::string fileName;
    std::vector<char> data;
};

} // namespace audio
} // namespace kola