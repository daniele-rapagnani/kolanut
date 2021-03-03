#include "kolanut/graphics/Renderer.h"
#include "kolanut/core/Logging.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/core/DIContainer.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <algorithm>

namespace kola {
namespace graphics {

namespace {

template <typename T, typename ...Args>
std::shared_ptr<T> loadWrapper(std::function<std::shared_ptr<T>(void)> creator, Args... args)
{
    std::shared_ptr<T> inst = creator();

    if (!inst->load(std::forward<Args>(args)...))
    {
        return nullptr;
    }

    return inst;
}

} // namespace

bool Renderer::init(const Config& config)
{
    this->designResolution = config.resolution.designResolution;
    this->config = config;

    if (!doInit(config))
    {
        return false;
    }

    this->program = createMainProgram();

    if (!this->program->link())
    {
        knM_logFatal("Can't link main program");
        return false;
    }

    GeometryBuffer::Config c = {};
    c.maxFrames = getConfig().framesInFlight;
    c.maxVerts = getConfig().maxGeometryBufferVertices;
    this->geometryBuffer = createGeometryBuffer();
    
    if (!this->geometryBuffer->init(c))
    {
        return false;
    }

    return true;
}

std::shared_ptr<Texture> Renderer::loadTexture(const std::string& file)
{
    knM_logDebug("Loading texture: " << file);
    return loadWrapper<Texture>(std::bind(&Renderer::createTexture, this), file);
}

std::shared_ptr<Font> Renderer::loadFont(const std::string& file, size_t size)
{
    knM_logDebug("Loading font: " << file);
    return loadWrapper<Font>(std::bind(&Renderer::createFont, this), file, size);
}

std::shared_ptr<Shader> Renderer::loadShader(
    const std::string& file, 
    Shader::Type type /* = Shader::Type::NONE */
)
{
    std::string newPath = file + getShadersExt();

    if (!di::get<filesystem::FilesystemEngine>()->isFile(newPath))
    {
        newPath = file;
    }

    knM_logDebug("Loading shader: " << newPath);
    return loadWrapper<Shader>(std::bind(&Renderer::createShader, this), newPath, type);
}

void Renderer::draw(
    std::shared_ptr<Texture> t, 
    const Vec2f& position, 
    float angle, 
    const Vec2f& scale,
    const Vec2f& origin
)
{
    Sizei ts = t->getSize();

    draw(
        t, 
        position,
        angle,
        scale,
        origin,
        Recti { 0, 0, ts.x, ts.y },
        {}
    );
}

void Renderer::draw(
    std::shared_ptr<Texture> t, 
    const Vec2f& position, 
    float angle, 
    const Vec2f& scale,
    const Vec2f& origin,
    const Recti& rect,
    const Colori& color
)
{
    assert(t);

    DrawTriangles dr = {};

    dr.texture = t;
    dr.program = getProgram();

    Sizei s = t->getSize();

    Rectf tcRect = { 
        rect.x / static_cast<float>(s.x), 
        rect.y / static_cast<float>(s.y),
        0.0f,
        0.0f
    };
    
    tcRect.z = tcRect.x + (rect.z / static_cast<float>(s.x));
    tcRect.w = tcRect.y + (rect.w / static_cast<float>(s.y));

    std::vector<Vertex> vertices = {
        { Vec2f { 0.0f, 0.0f }, { tcRect.x, tcRect.y }, color },
        { Vec2f { 0.0f, rect.w }, { tcRect.x, tcRect.w }, color },
        { Vec2f { rect.z, 0.0f }, { tcRect.z, tcRect.y }, color },
        { Vec2f { rect.z, 0.0f }, { tcRect.z, tcRect.y }, color },
        { Vec2f { 0.0f, rect.w }, { tcRect.x, tcRect.w }, color },
        { Vec2f { rect.z, rect.w }, { tcRect.z, tcRect.w }, color },
    };

    GeometryBuffer::Handle h = getGeometryBuffer()->addVertices(vertices, getCurrentFrame());
    dr.vertices = reinterpret_cast<void*>(h);
    dr.verticesCount = vertices.size();

    // T * S * R * O, outer comes first
    dr.transform = glm::translate(
        glm::rotate(
            glm::scale(
                glm::translate(
                    Transform3D { 1.0f },
                    glm::vec3 { position.x, position.y, 0.0f }
                ),
                glm::vec3 { scale.x, scale.y, 0.0f }
            ),
            static_cast<float>(angle * (M_PI / 180.0)),
            glm::vec3 { 0.0f, 0.0f, 1.0f }
        ),
        glm::vec3 { -origin.x, -origin.y, 0.0f }
    );

    float resScale = getResolution().x / getDesignResolution().x;

    dr.camera = glm::translate(
        glm::scale(
            Transform3D { 1.0f },
            glm::vec3 {
                resScale * getCameraZoom(),
                resScale * getCameraZoom(),
                1.0f
            }
        ),
        glm::vec3 {
            -getCameraPosition().x, 
            -getCameraPosition().y,
            0.0f
        }
    );

    drawTriangles(dr);
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

void Renderer::clear()
{
    getGeometryBuffer()->reset(getCurrentFrame());

    doClear();
}

void Renderer::flip()
{
    doFlip();

    this->currentInFlightFrame = 
        (this->currentInFlightFrame + 1) % 
        getConfig().framesInFlight
    ;
}

std::shared_ptr<Program> Renderer::createMainProgram()
{
    auto vertexShader = loadShader("assets/main.vert", Shader::Type::VERTEX);
    auto fragmentShader = loadShader("assets/main.frag", Shader::Type::FRAGMENT);

    assert(vertexShader);
    assert(fragmentShader);

    auto drawProgram = createProgram();
    drawProgram->addShader(fragmentShader);
    drawProgram->addShader(vertexShader);

    return drawProgram;
}

void Renderer::setCameraPosition(const Vec2f& pos)
{
    this->cameraPos = pos;
}

void Renderer::setCameraZoom(float zoom)
{
    this->cameraZoom = zoom;
}

Vec2f Renderer::getCameraPosition() const
{
    return this->cameraPos;
}

float Renderer::getCameraZoom() const
{
    return this->cameraZoom;
}

float Renderer::getPixelsPerPoint() const
{
    Vec2i r = getResolution();
    Vec2i pr = getPixelResolution();

    return pr.x / r.x;
}

} // namespace graphics
} // namespace kola