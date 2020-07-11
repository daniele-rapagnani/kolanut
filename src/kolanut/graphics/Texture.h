#pragma once

#include "kolanut/core/Types.h"

namespace kola {
namespace graphics {

class Texture
{ 
public:
    virtual bool load(const std::string& file) = 0;
    virtual Sizei getSize() = 0;
};

} // namespace graphics
} // namespace kola