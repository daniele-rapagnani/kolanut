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
    MiniaudioSound(const std::string& filename)
        : Sound(filename)
    { }

    std::vector<char>& getData()
    { return this->data; }

private:
    std::vector<char> data = {};
};

} // namespace audio
} // namespace kola