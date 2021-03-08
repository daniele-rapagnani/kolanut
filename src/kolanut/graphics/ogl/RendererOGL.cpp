#include <glad/glad.h>

#include "kolanut/graphics/ogl/RendererOGL.h"
#include "kolanut/graphics/ogl/TextureOGL.h"
#include "kolanut/graphics/ogl/FontOGL.h"
#include "kolanut/graphics/ogl/ShaderOGL.h"
#include "kolanut/graphics/ogl/ProgramOGL.h"
#include "kolanut/graphics/ogl/utils/GenericUtils.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/stats/StatsEngine.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"

#include <TracyOpenGL.hpp>
#include <Tracy.hpp>

#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

#include <cstring>
#include <sstream>
#include <cassert>
#include <cmath>

namespace kola {
namespace graphics {

RendererOGL::~RendererOGL()
{
    knM_oglCall(glDeleteQueries(1, &this->perfQuery));
}

bool RendererOGL::doInit(const Config& config)
{
    if (!RendererGLFW::doInit(config))
    {
        return false;
    }

    knM_logDebug("Initilizing OpenGL renderer");

    glfwMakeContextCurrent(getWindow());

    if (!gladLoadGL())
    {
        knM_logFatal("Can't initialize OpenGL.");
        return false;
    }

    TracyGpuContext

    onUpdateWindowSize();

    knM_oglCall(glDisable(GL_DEPTH_TEST));
    knM_oglCall(glEnable(GL_BLEND));
    knM_oglCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    knM_oglCall(glGenQueries(1, &this->perfQuery));

    return true;
}

void RendererOGL::onUpdateWindowSize()
{
    Sizei resolution = getPixelResolution();
    knM_oglCall(glViewport(0, 0, resolution.x, resolution.y));
}

void RendererOGL::drawSurface(const DrawSurface& req)
{
    auto h = reinterpret_cast<GeometryBuffer::Handle>(req.vertices);

    std::shared_ptr<ProgramOGL> program =
        std::static_pointer_cast<ProgramOGL>(req.program)
    ;

    std::shared_ptr<TextureOGL> texture = 
        std::static_pointer_cast<TextureOGL>(req.texture)
    ;

    auto glProgram = program->getGLProgram();

    glProgram->use();
    
    if (!glProgram->setUnifrom("transform", req.transform))
    {
        knM_logError("Can't find transform uniform in main shader");
        return;
    }

    glProgram->setUnifrom("screenSize", Vec2f { getDesignResolution().x, getDesignResolution().y });
    glProgram->setUnifrom("camera", req.camera);

    GLint posAtt = glProgram->getAttributeLocation("a_position");

    if (posAtt < 0)
    {
        knM_logError("Can't find a_position attribute in main vertex shader");
        return;
    }

    GLint tcAtt = glProgram->getAttributeLocation("a_texCoord");

    if (tcAtt < 0)
    {
        knM_logError("Can't find a_texCoord attribute in main vertex shader");
        return;
    }

    GLint colAtt = glProgram->getAttributeLocation("a_color");

    if (colAtt < 0)
    {
        knM_logError("Can't find a_color attribute in main vertex shader");
        return;
    }

    GLuint* buffers = reinterpret_cast<GLuint*>(req.vertices);

    void* offset = reinterpret_cast<void*>(this->geometryBuffer->getBase(h));
    void* offsetUv = reinterpret_cast<void*>(this->geometryBuffer->getBase(h) + sizeof(Vec2f));
    void* offsetColor = reinterpret_cast<void*>(this->geometryBuffer->getBase(h) + sizeof(Vec2f) * 2);

    std::static_pointer_cast<GeometryBufferOGL>(getGeometryBuffer())->bind();
    
    knM_oglCall(glEnableVertexAttribArray(posAtt));
    knM_oglCall(glVertexAttribPointer(posAtt, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offset));
    knM_oglCall(glEnableVertexAttribArray(tcAtt));
    knM_oglCall(glVertexAttribPointer(tcAtt, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetUv));
    knM_oglCall(glEnableVertexAttribArray(colAtt));
    knM_oglCall(glVertexAttribPointer(colAtt, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetColor));

    if (texture && texture != this->lastTexture)
    {
        texture->bind();
        this->lastTexture = texture;
    }

    switch(req.mode)
    {
        case DrawMode::TRIANGLES:
            knM_oglCall(glDrawArrays(GL_TRIANGLES, 0, req.verticesCount));
            break;

        case DrawMode::LINES:
            knM_oglCall(glDrawArrays(GL_LINES, 0, req.verticesCount));
            break;
    };
}

void RendererOGL::doClear()
{
    knM_oglCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    knM_oglCall(glBeginQuery(GL_TIME_ELAPSED_EXT, this->perfQuery));
}

void RendererOGL::doFlip()
{
    knM_oglCall(glEndQuery(GL_TIME_ELAPSED_EXT));

    RendererGLFW::doFlip();

    TracyGpuCollect

    if (glGetQueryObjectui64vEXT)
    {
        GLuint64 elapsed = 0;
        knM_oglCall(glGetQueryObjectui64vEXT(this->perfQuery, GL_QUERY_RESULT, &elapsed));

        double e = static_cast<double>(elapsed) / 1000000.0;

        auto stats = di::get<stats::StatsEngine>();
        stats->addSample(stats::StatsEngine::Measure::GPU_TIME, e);
    }
}

std::shared_ptr<Texture> RendererOGL::createTexture()
{
    return std::make_shared<TextureOGL>();
}

std::shared_ptr<Font> RendererOGL::createFont()
{
    return std::make_shared<FontOGL>();
}

std::shared_ptr<Shader> RendererOGL::createShader()
{
    return std::make_shared<ShaderOGL>();
}

std::shared_ptr<Program> RendererOGL::createProgram(DrawMode mode)
{
    return std::make_shared<ProgramOGL>();
}

std::shared_ptr<GeometryBuffer> RendererOGL::createGeometryBuffer()
{
    return std::make_shared<GeometryBufferOGL>();
}

} // namespace graphics
} // namespace kola