#pragma once

#include "kolanut/audio/Audio.h"

#include <cstdint>
#include <string>

namespace kola {
namespace audio {

class Sound
{ 
public:
    Sound(const std::string filename)
        : filename(filename)
    { }

    const std::string& getFilename() const
    { return this->filename; }

private:
    std::string filename = {};
};

} // namespace audio
} // namespace kola