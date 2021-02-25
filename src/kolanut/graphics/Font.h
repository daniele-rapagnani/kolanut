#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/graphics/Texture.h"

#include <memory>

namespace kola {
namespace graphics {

class Font
{
public:
    struct Glyph
    {
        float xOffset = 0.0f;
        float yOffset = 0.0f;
        float xAdvance = 0.0f;
        Recti textureCoords = {};
    };

public:
    static const size_t DEF_SIZE = 16;

public:
    virtual bool load(const std::string& file, size_t size) = 0;
    virtual const std::string& getFilename() const = 0;
    virtual const Glyph* getGlyphInfo(int codepoint) const = 0;
    virtual Sizei getTextSize(const std::string& str) const;
    virtual Sizei getTextSize(const char* str, size_t len) const;
    virtual size_t getNativeFontSize() const = 0;
    virtual std::shared_ptr<Texture> getTexture() const = 0;
};

} // namespace graphics
} // namespace kola