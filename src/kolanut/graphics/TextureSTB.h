#pragma once

#include "kolanut/graphics/Texture.h"

namespace kola {
namespace graphics {

class TextureSTB : public Texture
{ 
public:
    virtual bool load(const std::string& file) override;
    virtual Sizei getSize() override;

    virtual bool load(unsigned char* data, size_t w, size_t h)
    {
        this->size = { w, h };
        return doLoad(data, w, h);
    }
    
    virtual bool doLoad(unsigned char* data, size_t w, size_t h) = 0;

    bool isDataOwned() const
    { return this->dataOwned; }

    const unsigned char* getData() const
    { return this->data; }

protected:
    void setDataOwned(bool ownsData)
    { this->dataOwned = ownsData; }
    
private:
    Sizei size = {};
    unsigned char* data = nullptr;
    bool dataOwned = false;
};

} // namespace graphics
} // namespace kola