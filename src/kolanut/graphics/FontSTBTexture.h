#pragma once

#include "kolanut/graphics/FontSTB.h"
#include "kolanut/core/Logging.h"

#include <memory>

namespace kola {
namespace graphics {

template <typename T>
class FontSTBTexture : public FontSTB
{
public:
    bool createTexture() override
    {
        this->texture = std::make_shared<T>();

        if (!this->texture->load(
            reinterpret_cast<unsigned char*>(getTextureData()), 
            getAtlasSize().x, 
            getAtlasSize().y
        ))
        {
            knM_logError("Can't load texture from font atlas");
            return false;
        }
        
        return true;
    }

    std::shared_ptr<Texture> getTexture() const override
    { return std::static_pointer_cast<Texture>(this->texture); }

private:
    std::shared_ptr<T> texture = nullptr;
};

} // namespace graphics
} // namespace kola