#include "kolanut/graphics/Font.h"

#include <string>

namespace kola {
namespace graphics {

Sizei Font::getTextSize(const std::string& str) const
{
    return getTextSize(str.c_str(), str.size());
}

Sizei Font::getTextSize(const char* str, size_t len) const
{
    Sizei r = {};

    long lineHeight = 0;
    long advance = 0;

    for (size_t i = 0; i < len; i++)
    {
        if (str[i] == '\n')
        {
            r.y += getNativeFontSize();
            r.x = std::max(r.x, advance);

            lineHeight = 0;
            advance = 0;

            continue;
        }

        const Font::Glyph* g = getGlyphInfo(str[i]);

        if (!g)
        {
            continue;
        }
        
        lineHeight = std::max(g->textureCoords.w, lineHeight);
        advance += g->xAdvance;
    }

    r.y += lineHeight;
    r.x = std::max(r.x, advance);

    return r;
}

} // namespace graphics
} // namespace kola