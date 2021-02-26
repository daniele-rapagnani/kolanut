#include "kolanut/graphics/ogl/RendererOGL.h"
#include "kolanut/graphics/ogl/TextureOGL.h"
#include "kolanut/graphics/ogl/FontOGL.h"
#include "kolanut/graphics/ogl/utils/GenericUtils.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"

#include <glad/glad.h>
#include <TracyOpenGL.hpp>

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

    if (this->window)
    {
        glfwDestroyWindow(this->window);
        this->window = nullptr;
    }
}

namespace {

void onWindowResize(GLFWwindow* window, int width, int height)
{
    auto r = std::static_pointer_cast<RendererOGL>(di::get<Renderer>());
    r->updateWindowSize();
}

} // namespace 

bool RendererOGL::doInit(const Config& config)
{
    knM_logDebug("Initilizing OpenGL renderer");

    if (!glfwInit())
    {
        knM_logFatal("Can't init GLFW");
        return false;
    }

    GLFWmonitor* monitor = nullptr;

    if (config.resolution.fullScreen)
    {
        int monitorsCount = 0;
        GLFWmonitor** monitors = glfwGetMonitors(&monitorsCount);

        if (!monitors || monitorsCount == 0)
        {
            knM_logFatal("GLFW couldn't enumerate monitors or none is connected.");
            return false;
        }

        monitor = monitors[0];
    }

    this->window = glfwCreateWindow(
        config.resolution.screenSize.x,
        config.resolution.screenSize.y,
        config.windowTitle.c_str(), 
        monitor, 
        NULL
    );

    if (!this->window)
    {
        glfwTerminate();

        knM_logFatal("Can't create window with GLFW");
        return false;
    }

    glfwSetWindowAspectRatio(
        this->window, 
        config.resolution.screenSize.x, 
        config.resolution.screenSize.y
    );

    glfwMakeContextCurrent(this->window);

    if (!gladLoadGL())
    {
        knM_logFatal("Can't initialize OpenGL.");
        return false;
    }

    TracyGpuContext

    updateWindowSize();

    knM_oglCall(glDisable(GL_DEPTH_TEST));
    knM_oglCall(glEnable(GL_BLEND));
    knM_oglCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    std::string error;

    std::shared_ptr<utils::ogl::Shader> drawFragShader = 
        this->loadShader("assets/main.frag", error)
    ;

    if (!drawFragShader)
    {
        knM_logFatal("Can't load main frag shader: " << std::endl << error);
        return false;
    }

    std::shared_ptr<utils::ogl::Shader> drawVertShader = 
        this->loadShader("assets/main.vert", error)
    ;

    if (!drawVertShader)
    {
        knM_logFatal("Can't load main vert shader: " << std::endl << error);
        return false;
    }

    this->drawProgram = std::make_shared<utils::ogl::Program>();

    this->drawProgram->attach(drawFragShader);
    this->drawProgram->attach(drawVertShader);

    if (!this->drawProgram->link(error))
    {
        knM_logFatal("Can't link main program: " << error);
        return false;
    }

    glfwSetWindowSizeCallback(getWindow(), onWindowResize);

    knM_oglCall(glGenQueries(1, &this->perfQuery));

    return true;
}

void RendererOGL::updateWindowSize()
{
    Sizei resolution = getPixelResolution();
    knM_oglCall(glViewport(0, 0, resolution.x, resolution.y));
}

namespace {

bool checkExtension(const std::string& str, const std::string& ext)
{
    if (str.size() < ext.size())
    {
        return false;
    }

    for (size_t i = 0; i < ext.size(); i++)
    {
        if (str[str.size() - i - 1] != ext[ext.size() - i - 1])
        {
            return false;
        }
    }

    return true;
}

} // namespace

std::shared_ptr<utils::ogl::Shader> RendererOGL::loadShader(
    const std::string& path,
    std::string& error,
    utils::ogl::Shader::Type type /* = utils::ogl::Shader::Type::NONE */
)
{
    std::string shaderSource;
    std::string compError;

    std::shared_ptr<utils::ogl::Shader> shader = nullptr;
    
    if (!di::get<filesystem::FilesystemEngine>()->getFileContent(path, shaderSource))
    {
        std::stringstream ss;
        ss << "Can't find shader: " << path;
        error = ss.str();

        return nullptr;
    }

    if (type == utils::ogl::Shader::Type::NONE)
    {
        if (checkExtension(path, RendererOGL::SHADER_FRAG_EXT))
        {
            type = utils::ogl::Shader::Type::FRAGMENT;
        }
        else if (checkExtension(path, RendererOGL::SHADER_VERT_EXT))
        {
            type = utils::ogl::Shader::Type::VERTEX;
        }
        else
        {
            std::stringstream ss;
            ss << "Can't guess shader extension from path: " << path;
            error = ss.str();

            return nullptr;
        }
    }

    shader = std::make_shared<utils::ogl::Shader>(
        shaderSource, 
        type
    );

    if (!shader->compile(compError))
    {
        std::stringstream ss;
        ss << "Can't compile shader (" << path << "): " << std::endl << compError;
        error = ss.str();

        return nullptr;
    }

    return shader;
}

std::shared_ptr<Texture> RendererOGL::loadTexture(const std::string& file)
{
    knM_logDebug("Loading texture: " << file);
    
    std::shared_ptr<TextureOGL> texture = std::make_shared<TextureOGL>();
    
    if (!texture->load(file))
    {
        return nullptr;
    }

    return std::static_pointer_cast<Texture>(texture);
}

std::shared_ptr<Font> RendererOGL::loadFont(const std::string& file, size_t size)
{
    knM_logDebug("Loading font: " << file);

    std::shared_ptr<FontOGL> font = std::make_shared<FontOGL>();
    
    if (!font->load(file, size))
    {
        return nullptr;
    }

    return std::static_pointer_cast<Font>(font);
}

void RendererOGL::draw(
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

void RendererOGL::draw(
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

    utils::ogl::QuadDrawRequest dr;

    dr.transform = glm::translate(
        glm::scale(
            glm::rotate(
                glm::translate(
                    glm::identity<Transform2D>(), 
                    glm::vec2 { position.x, position.y }
                ), 
                static_cast<float>(angle * (M_PI / 180.0))
            ),
            { scale.x, scale.y }
        ),
        glm::vec2 { -origin.x, -origin.y }
    );

    this->drawProgram->use();

    Sizei r = getResolution();
    float resScale = r.x / getDesignResolution().x;

    Transform2D cameraTransform = glm::translate(
        glm::scale(
            glm::identity<Transform2D>(),
            glm::vec2 {
                resScale * this->cameraZoom,
                resScale * this->cameraZoom
            }
        ),
        glm::vec2 {
            -this->cameraPos.x, 
            -this->cameraPos.y
        }
    );

    Vec2i resolution = getResolution();
    this->drawProgram->setUnifrom("screenSize", Vec2f { getDesignResolution().x, getDesignResolution().y });
    this->drawProgram->setUnifrom("camera", cameraTransform);

    dr.texture = std::static_pointer_cast<TextureOGL>(t);
    dr.program = this->drawProgram;
    dr.textureRect = { rect.x, rect.y, rect.z, rect.w };

    utils::ogl::draw(dr);
}

void RendererOGL::draw(
    const Rectf& rect,
    const Colori& color
)
{

}

void RendererOGL::draw(
    const Vec2f& a,
    const Vec2f& b,
    const Colori& color
)
{

}

void RendererOGL::clear()
{
    knM_oglCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    knM_oglCall(glBeginQuery(GL_TIME_ELAPSED_EXT, this->perfQuery));
}

void RendererOGL::flip()
{
    knM_oglCall(glEndQuery(GL_TIME_ELAPSED_EXT));

    assert(this->window);
    glfwSwapBuffers(this->window);

    TracyGpuCollect

    if (glGetQueryObjectui64vEXT)
    {
        GLuint64 elapsed = 0;
        knM_oglCall(glGetQueryObjectui64vEXT(this->perfQuery, GL_QUERY_RESULT, &elapsed));

        this->gpuElapsed += static_cast<double>(elapsed) / 1000000.0;
        this->gpuSamples++;
    }

    if (this->gpuSamples == 60)
    {
        knM_logDebug(
            "[OpenGL] rendering took an avg of " 
            << (this->gpuElapsed / this->gpuSamples) 
            << " ms for 60 frames"
        );

        this->gpuSamples = 0;
        this->gpuElapsed = 0.0f;
    }
}

void RendererOGL::setCameraPosition(const Vec2f& pos)
{
    this->cameraPos = pos;
}

void RendererOGL::setCameraZoom(float zoom)
{
    this->cameraZoom = zoom;
}

Vec2f RendererOGL::getCameraPosition()
{
    return this->cameraPos;
}

float RendererOGL::getCameraZoom()
{
    return this->cameraZoom;
}

Vec2i RendererOGL::getResolution()
{
    assert(this->window);

    int w, h;
    glfwGetWindowSize(this->window, &w, &h);

    return { w, h };
}

Vec2i RendererOGL::getPixelResolution()
{
    assert(this->window);

    int w, h;
    glfwGetFramebufferSize(this->window, &w, &h);

    return { w, h };
}

uint8_t RendererOGL::getPixelsPerPoint()
{
    Vec2i r = getResolution();
    Vec2i pr = getPixelResolution();

    return pr.x / r.x;
}

} // namespace graphics
} // namespace kola