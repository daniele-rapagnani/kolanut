#include "kolanut/graphics/Renderer.h"

#include <cassert>
#include <algorithm>

namespace kola {
namespace graphics {

bool Renderer::init(const Config& config)
{
    this->designResolution = config.resolution.designResolution;

    return doInit(config);
}

void Renderer::draw(
    const char* str,
    size_t len,
    std::shared_ptr<Font> f, 
    const Vec2f& position, 
    const Vec2f& scale,
    const Colori& color
)
{
    assert(f);

    if (!f)
    {
        return;
    }

    Vec2f curPos = position;

    for (size_t i = 0; i < len; i++)
    {
        if (str[i] == '\n')
        {
            curPos.x = 0;
            curPos.y += f->getNativeFontSize() * scale.y;
            continue;
        }

        const Font::Glyph* g = f->getGlyphInfo(str[i]);

        if (!g)
        {
            continue;
        }

        Vec2f glyphPos = curPos + Vec2f(g->xOffset * scale.x, g->yOffset * scale.y);

        draw(f->getTexture(), glyphPos, 0.0f, scale, {}, g->textureCoords, color);
        curPos.x += g->xAdvance * scale.x;
    }
}

} // namespace graphics
} // namespace kola