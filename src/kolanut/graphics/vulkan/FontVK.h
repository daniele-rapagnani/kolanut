#pragma once

#include "kolanut/graphics/Font.h"
#include "kolanut/graphics/vulkan/TextureVK.h"

#define STBTT_STATIC
#include <stb_truetype.h>

#include <string>
#include <memory>
#include <vector>
#include <cstdint>

namespace kola {
namespace graphics {

class RendererVK;

class FontVK : public Font
{
public:
    static const size_t DEF_ATLAS_WIDTH = 512;
    static const size_t DEF_ATLAS_HEIGHT = 512;
    static const size_t DEF_FIRST_CODEPOINT = 1;
    static const size_t DEF_LAST_CODEPOINT = 255;

public:
    friend class RendererVK;

    FontVK() = default;
    ~FontVK();

public:
    bool load(const std::string& file, size_t size = DEF_SIZE) override;

    const Glyph* getGlyphInfo(int codepoint) const override;

    const std::string& getFilename() const override
    { return this->file; }

    std::shared_ptr<Texture> getTexture() const override
    { return std::static_pointer_cast<Texture>(this->texture); }

    size_t getNativeFontSize() const override
    { return this->size; }

private:
    Glyph createGlyphInfo(int codepoint) const;

    size_t getCodepointsCount() const
    { 
        return this->lastCodepoint - this->firstCodepoint;
    }

    size_t getIndexFromCodepoint(int codepoint) const
    {
        return codepoint - this->firstCodepoint;
    }

private:
    std::string file;
    size_t size = DEF_SIZE;
    size_t firstCodepoint = DEF_FIRST_CODEPOINT;
    size_t lastCodepoint = DEF_LAST_CODEPOINT;
    uint32_t* textureData = nullptr;
    Sizei atlasSize = { DEF_ATLAS_WIDTH, DEF_ATLAS_HEIGHT };
    std::shared_ptr<TextureVK> texture = nullptr;
    std::vector<stbtt_packedchar> charsMap;
    std::vector<Glyph> glyphs;
};

} // namespace graphics
} // namespace kola