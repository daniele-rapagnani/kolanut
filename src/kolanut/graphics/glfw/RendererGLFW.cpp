#include "kolanut/graphics/glfw/RendererGLFW.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"

namespace kola {
namespace graphics {

RendererGLFW::~RendererGLFW()
{
    if (this->window)
    {
        glfwDestroyWindow(this->window);
        this->window = nullptr;
    }
}

namespace {

void onWindowResize(GLFWwindow* window, int width, int height)
{
    auto r = std::static_pointer_cast<RendererGLFW>(di::get<Renderer>());
    r->onUpdateWindowSize();
}

} // namespace 

bool RendererGLFW::doInit(const Config& config)
{
    knM_logDebug("Initilizing GLFW");

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

    if (isUseNoAPI())
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
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

        const char* err = nullptr;
#ifndef __EMSCRIPTEN__
        glfwGetError(&err);
#endif

        knM_logFatal("Can't create window with GLFW: " << (err ? err : "Unknown"));
        return false;
    }

    glfwSetWindowAspectRatio(
        this->window, 
        config.resolution.screenSize.x, 
        config.resolution.screenSize.y
    );

    glfwSetWindowSizeCallback(getWindow(), onWindowResize);

    return true;
}

void RendererGLFW::doFlip()
{
    assert(this->window);
    glfwSwapBuffers(this->window);
}

Sizei RendererGLFW::getResolution() const
{
    assert(this->window);

    int w, h;
    glfwGetWindowSize(this->window, &w, &h);

    return { static_cast<size_t>(w), static_cast<size_t>(h) };
}

Sizei RendererGLFW::getPixelResolution() const
{
    assert(this->window);

    int w, h;
    glfwGetFramebufferSize(this->window, &w, &h);

    return { static_cast<size_t>(w), static_cast<size_t>(h) };
}

} // namespace graphics
} // namespace kola