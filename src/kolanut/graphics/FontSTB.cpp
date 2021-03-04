#include "kolanut/graphics/FontSTB.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"

#define STBRP_STATIC
#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC

#include <stb_rect_pack.h>
#include <stb_truetype.h>

#include <vector>
#include <memory>

namespace kola {
namespace graphics {

FontSTB::~FontSTB()
{
    if (this->textureData)
    {
        delete[] this->textureData;
        this->textureData = nullptr;
    }
}

bool FontSTB::load(const std::string& file, size_t size)
{
    this->size = size;

    std::vector<char> buffer;

    if (!di::get<filesystem::FilesystemEngine>()->getFileContent(file, buffer))
    {
        knM_logError("Can't read font file: '" << file << "'");
        return false;
    }

    const unsigned char* fontData = reinterpret_cast<const unsigned char*>(&buffer[0]); 

    uint32_t w = this->atlasSize.x;
    uint32_t h = this->atlasSize.y;

    stbtt_pack_context pack;
    unsigned char* dataMono = new unsigned char[w*h];

    if (!stbtt_PackBegin(&pack, dataMono, w, h, 0, 1, nullptr))
    {
        knM_logError("Can't begin font packing: '" << file << "'");
        return false;
    }

    this->charsMap.resize(getCodepointsCount());
    this->glyphs.resize(getCodepointsCount());

    stbtt_pack_range ranges = {};
    ranges.font_size = this->size;
    ranges.first_unicode_codepoint_in_range = this->firstCodepoint;
    ranges.num_chars = getCodepointsCount();
    ranges.chardata_for_range = &this->charsMap[0];

    if (!stbtt_PackFontRanges(&pack, fontData, 0, &ranges, 1))
    {
        knM_logError("Can't pack font range: '" << file << "'");
        return false;
    }

    stbtt_PackEnd(&pack);

    if (this->textureData)
    {
        delete[] this->textureData;
        this->textureData = nullptr;
    }

    this->textureData = new uint32_t[w*h];

    size_t pixels = w * h;

    for (size_t i = 0; i < pixels; i++)
    {
        unsigned char p = (dataMono[i]);

        this->textureData[i] = (p << 24) | (p << 16) | (p << 8) | p;
    }

    delete[] dataMono;

    if (!createTexture())
    {
        return false;
    }

    for (size_t i = this->firstCodepoint; i < this->lastCodepoint; i++)
    {
        this->glyphs[getIndexFromCodepoint(i)] = createGlyphInfo(i);
    }

    return true;
}

Font::Glyph FontSTB::createGlyphInfo(int codepoint) const
{
    Glyph g;

    Vec2f pos = {};
    stbtt_aligned_quad q = {};

    size_t idx = getIndexFromCodepoint(codepoint);
    assert(idx < this->charsMap.size());

    stbtt_GetPackedQuad(
        &this->charsMap[0], 
        this->atlasSize.x, 
        this->atlasSize.y, 
        idx, 
        &pos.x, 
        &pos.y, 
        &q, 
        1
    );

    Recti& r = g.textureCoords;

    r = { 
        static_cast<int32_t>(this->atlasSize.x * q.s0), 
        static_cast<int32_t>(this->atlasSize.y * q.t0), 
        static_cast<int32_t>(this->atlasSize.x * q.s1), 
        static_cast<int32_t>(this->atlasSize.y * q.t1)
    };

    r.z -= r.x;
    r.w -= r.y;

    g.xAdvance = this->charsMap[idx].xadvance;
    g.xOffset = this->charsMap[idx].xoff;
    g.yOffset = (getNativeFontSize() / 2) + this->charsMap[idx].yoff;

    return g;
}

const Font::Glyph* FontSTB::getGlyphInfo(int codepoint) const
{
    if (codepoint >= this->glyphs.size())
    {
        knM_logDebug("Missing char from font '" << getFilename() << "'. Codepoint = " << codepoint);
        return nullptr;
    }

    return &this->glyphs[getIndexFromCodepoint(codepoint)];
}

} // namespace graphics
} // namespace kola