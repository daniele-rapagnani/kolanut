#include "kolanut/graphics/Renderer.h"
#include "kolanut/core/Logging.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/stats/StatsEngine.h"
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
    this->lineProgram = createLineProgram();

    if (!this->program->link())
    {
        knM_logFatal("Can't link main program");
        return false;
    }

    if (!this->lineProgram->link())
    {
        knM_logFatal("Can't link line program");
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

    this->jobs.reserve(getConfig().jobQueueInitialSize);

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
    const Transform3D& tr,
    const Recti& rect,
    const Colori& color
)
{
    assert(t);

    DrawSurface ds;

    ds.texture = t;
    ds.program = getProgram();

    Sizei s = t->getSize();

    Rectf tcRect = { 
        {
            rect.origin.x / static_cast<float>(s.x), 
            rect.origin.y / static_cast<float>(s.y)
        },
        {
            0.0f,
            0.0f
        }
    };
    
    tcRect.size.x = tcRect.origin.x + (rect.size.x / static_cast<float>(s.x));
    tcRect.size.y = tcRect.origin.y + (rect.size.y / static_cast<float>(s.y));

    std::vector<Vertex> vertices = {
        { Vec2f { 0.0f, 0.0f }, { tcRect.origin.x, tcRect.origin.y }, color },
        { Vec2f { 0.0f, rect.size.y }, { tcRect.origin.x, tcRect.size.y }, color },
        { Vec2f { rect.size.x, 0.0f }, { tcRect.size.x, tcRect.origin.y }, color },
        { Vec2f { rect.size.x, 0.0f }, { tcRect.size.x, tcRect.origin.y }, color },
        { Vec2f { 0.0f, rect.size.y }, { tcRect.origin.x, tcRect.size.y }, color },
        { Vec2f { rect.size.x, rect.size.y }, { tcRect.size.x, tcRect.size.y }, color },
    };

    GeometryBuffer::Handle h = getGeometryBuffer()->addVertices(vertices, getCurrentFrame());
    ds.vertices = reinterpret_cast<void*>(h);
    ds.verticesCount = vertices.size();
    ds.transform = tr;
    ds.camera = this->cameraTransform;

    enqueueDraw(ds);
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

        Transform3D tr = glm::scale(
            glm::translate(
                Transform3D { 1.0f },
                glm::vec3 { glyphPos.x, glyphPos.y, 0.0f }
            ),
            glm::vec3 { scale.x, scale.y, 0.0f }
        );

        draw(
            f->getTexture(),
            tr,
            g->textureCoords,
            color
        );

        curPos.x += g->xAdvance * scale.x;
    }
}

void Renderer::draw(
    const Vec2f& a,
    const Vec2f& b,
    const Colori& color
)
{
    Vertex vertices[2] = {
        { Vec2f { a.x, a.y }, { 0.0f, 0.0f }, color },
        { Vec2f { b.x, b.y }, { 0.0f, 0.0f }, color },
    };

    DrawSurface ds;

    GeometryBuffer::Handle h = getGeometryBuffer()->addVertices(vertices, 2, getCurrentFrame());
    ds.transform = Transform3D { 1.0f };
    ds.mode = DrawMode::LINES;
    ds.vertices = reinterpret_cast<void*>(h);
    ds.verticesCount = 2;
    ds.texture = nullptr;
    ds.program = getLineProgram();
    ds.camera = this->cameraTransform;
}

void Renderer::draw(
    const Rectf& rect,
    const Colori& color
)
{
    DrawSurface ds;

    ds.texture = nullptr;
    ds.program = getLineProgram();

    Vertex vertices[8] = {
        { rect.origin, { 0.0f, 0.0f }, color },
        { rect.origin + Vec2f{ rect.size.x, 0.0f }, { 0.0f, 0.0f }, color },

        { rect.origin + Vec2f{ rect.size.x, 0.0f }, { 0.0f, 0.0f }, color },
        { rect.origin + Vec2f{ rect.size.x, rect.size.y }, { 0.0f, 0.0f }, color },

        { rect.origin + Vec2f{ rect.size.x, rect.size.y }, { 0.0f, 0.0f }, color },
        { rect.origin + Vec2f{ 0.0f, rect.size.y }, { 0.0f, 0.0f }, color },

        { rect.origin + Vec2f{ 0.0f, rect.size.y }, { 0.0f, 0.0f }, color },
        { rect.origin, { 0.0f, 0.0f }, color },
    };

    GeometryBuffer::Handle h = getGeometryBuffer()->addVertices(vertices, 8, getCurrentFrame());
    ds.vertices = reinterpret_cast<void*>(h);
    ds.verticesCount = 8;
    ds.transform = Transform3D { 1.0f };
    ds.mode = DrawMode::LINES;
    ds.camera = this->cameraTransform;

    enqueueDraw(ds);
}

void Renderer::enqueueDraw(DrawSurface ds)
{
    if (!jobs.empty())
    {
        if (ds.isBatchableWith(jobs.back()))
        {
            DrawSurface& pdf = jobs.back();
            pdf.verticesCount += ds.verticesCount;
            
            di::get<stats::StatsEngine>()
                ->addToCurrentSample(stats::StatsEngine::Measure::BATCHED_DRAWS, 1)
            ;
            
            return;
        }
    }

    jobs.push_back(std::move(ds));
}

void Renderer::updateCameraTransform() const
{
    float resScale = getResolution().x / getDesignResolution().x;

    this->cameraTransform = glm::translate(
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
}

void Renderer::clear()
{
    this->jobs.clear();
    di::get<stats::StatsEngine>()->addSample(stats::StatsEngine::Measure::BATCHED_DRAWS, 0);
    getGeometryBuffer()->reset(getCurrentFrame());

    doClear();
}

void Renderer::flip()
{
    auto stats = di::get<stats::StatsEngine>();
    stats->addSample(stats::StatsEngine::Measure::TRIANGLES, 0);
    stats->addSample(stats::StatsEngine::Measure::DRAW_CALLS, 0);

    for (const auto& job : this->jobs)
    {
        stats->addToCurrentSample(stats::StatsEngine::Measure::TRIANGLES, job.verticesCount);
        stats->addToCurrentSample(stats::StatsEngine::Measure::DRAW_CALLS, 1);

        drawSurface(job);
    }

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

    auto drawProgram = createProgram(DrawMode::TRIANGLES);
    drawProgram->addShader(fragmentShader);
    drawProgram->addShader(vertexShader);

    return drawProgram;
}

std::shared_ptr<Program> Renderer::createLineProgram()
{
    auto vertexShader = loadShader("assets/main.vert", Shader::Type::VERTEX);
    auto fragmentShader = loadShader("assets/solid.frag", Shader::Type::FRAGMENT);

    assert(vertexShader);
    assert(fragmentShader);

    auto drawProgram = createProgram(DrawMode::LINES);
    drawProgram->addShader(fragmentShader);
    drawProgram->addShader(vertexShader);

    return drawProgram;
}

void Renderer::setCameraPosition(const Vec2f& pos)
{
    this->cameraPos = pos;
    updateCameraTransform();
}

void Renderer::setCameraZoom(float zoom)
{
    this->cameraZoom = zoom;
    updateCameraTransform();
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
    Sizei r = getResolution();
    Sizei pr = getPixelResolution();

    return pr.x / r.x;
}

} // namespace graphics
} // namespace kola