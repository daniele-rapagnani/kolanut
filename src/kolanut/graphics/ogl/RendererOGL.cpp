#include "kolanut/graphics/ogl/OpenGL.h"

#include "kolanut/graphics/ogl/RendererOGL.h"
#include "kolanut/graphics/ogl/TextureOGL.h"
#include "kolanut/graphics/ogl/FontOGL.h"
#include "kolanut/graphics/ogl/ShaderOGL.h"
#include "kolanut/graphics/ogl/ProgramOGL.h"
#include "kolanut/graphics/ogl/utils/Context.h"
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
#ifndef __EMSCRIPTEN__
    if (this->perfQuery > 0)
    {
        knM_oglCall(glDeleteQueries(1, &this->perfQuery));
    }
#endif
}

bool RendererOGL::doInit(const Config& config)
{
    if (!RendererGLFW::doInit(config))
    {
        return false;
    }

    di::registerType<utils::ogl::Context>([] {
        return std::make_shared<utils::ogl::Context>();
    });

    knM_logDebug("Initilizing OpenGL renderer");

    glfwMakeContextCurrent(getWindow());

#ifndef __EMSCRIPTEN__
    if (!gladLoadGL())
    {
        knM_logFatal("Can't initialize OpenGL.");
        return false;
    }
#endif

    TracyGpuContext

    onUpdateWindowSize();

    knM_oglCall(glDisable(GL_DEPTH_TEST));
    knM_oglCall(glEnable(GL_BLEND));
    knM_oglCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

#ifndef __EMSCRIPTEN__
    knM_oglCall(glGenQueries(1, &this->perfQuery));
#endif

    return true;
}

bool RendererOGL::init(const Config& config)
{
    if (!RendererGLFW::init(config))
    {
        return false;
    }

    bindProgramAttributesLocations(getProgram());
    bindProgramAttributesLocations(getLineProgram());

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

    void* offset = reinterpret_cast<void*>(this->geometryBuffer->getBase(h));
    void* offsetUv = reinterpret_cast<void*>(this->geometryBuffer->getBase(h) + sizeof(Vec2f));
    void* offsetColor = reinterpret_cast<void*>(this->geometryBuffer->getBase(h) + sizeof(Vec2f) * 2);

    std::static_pointer_cast<GeometryBufferOGL>(getGeometryBuffer())->bind();
    
    knM_oglCall(glEnableVertexAttribArray(POS_ATTR_LOC));
    knM_oglCall(glVertexAttribPointer(POS_ATTR_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offset));
    knM_oglCall(glEnableVertexAttribArray(TC_ATTR_LOC));
    knM_oglCall(glVertexAttribPointer(TC_ATTR_LOC, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetUv));
    knM_oglCall(glEnableVertexAttribArray(COLOR_ATTR_LOC));
    knM_oglCall(glVertexAttribPointer(COLOR_ATTR_LOC, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetColor));

    if (texture)
    {
        texture->bind();
    }

    GLuint firstVert = 0; //this->geometryBuffer->getBase(h) / sizeof(Vertex);

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

#ifndef __EMSCRIPTEN__
    knM_oglCall(glBeginQuery(GL_TIME_ELAPSED_EXT, this->perfQuery));
#endif
}

void RendererOGL::doFlip()
{
#ifndef __EMSCRIPTEN__
    knM_oglCall(glEndQuery(GL_TIME_ELAPSED_EXT));
#endif

    RendererGLFW::doFlip();

    TracyGpuCollect

#ifndef __EMSCRIPTEN__
    if (glGetQueryObjectui64vEXT)
    {
        GLuint64 elapsed = 0;
        knM_oglCall(glGetQueryObjectui64vEXT(this->perfQuery, GL_QUERY_RESULT, &elapsed));

        double e = static_cast<double>(elapsed) / 1000000.0;

        auto stats = di::get<stats::StatsEngine>();
        stats->addSample(stats::StatsEngine::Measure::GPU_TIME, e);
    }
#endif
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

void RendererOGL::bindProgramAttributesLocations(std::shared_ptr<Program> program) const
{
    auto glp = std::static_pointer_cast<ProgramOGL>(program)->getGLProgram();
    
    if (!glp->bindAttributeToLocation(POS_ATTR_NAME, POS_ATTR_LOC))
    {
        knM_logError("Can't find " << POS_ATTR_NAME << " attribute in vertex shader");
    }

    if (!glp->bindAttributeToLocation(TC_ATTR_NAME, TC_ATTR_LOC))
    {
        knM_logError("Can't find " << TC_ATTR_NAME << " attribute in vertex shader");
    }

    if (!glp->bindAttributeToLocation(COLOR_ATTR_NAME, COLOR_ATTR_LOC))
    {
        knM_logError("Can't find " << COLOR_ATTR_NAME << " attribute in vertex shader");
    }

    std::string linkErr;

    if (!glp->link(linkErr))
    {
        knM_logError("Can't link program with new bindings: " << linkErr);
    }
}

std::shared_ptr<GeometryBuffer> RendererOGL::createGeometryBuffer()
{
    return std::make_shared<GeometryBufferOGL>();
}

} // namespace graphics
} // namespace kola