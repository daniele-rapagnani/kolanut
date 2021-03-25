#include "kolanut/graphics/glfw/RendererGLFW.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"

namespace kola {
namespace graphics {

namespace {

void onWindowResize(GLFWwindow* window, int width, int height)
{
    auto r = std::static_pointer_cast<RendererGLFW>(di::get<Renderer>());
    
    knM_logDebug(
        "New window size: " 
        << r->getPixelResolution().x << ", " << r->getPixelResolution().y
    );

    r->updateViewport(r->getPixelResolution());
    r->onUpdateWindowSize();
}

Sizei adjustScreenSize(Sizei currentSize, float desiredRatio, ResolutionFitMode mode)
{
    switch (mode)
    {
        case ResolutionFitMode::NONE:
        case ResolutionFitMode::STRETCH:
            return currentSize;

        default:
            break;
    }

    Sizei newSize = currentSize;
    newSize.x = std::round(newSize.y * desiredRatio);

    if (
        ((newSize.x > currentSize.x) && mode == ResolutionFitMode::CONTAIN)
        || ((newSize.x < currentSize.x) && mode == ResolutionFitMode::COVER)
    )
    {
        newSize.x = currentSize.x;
        newSize.y = std::round(newSize.x * (1.0f / desiredRatio));
    }

    return newSize;
}

} // namespace 

RendererGLFW::~RendererGLFW()
{
    if (this->window)
    {
        glfwDestroyWindow(this->window);
        this->window = nullptr;
        
        glfwTerminate();
    }
}

void RendererGLFW::updateViewport(Sizei newScreenSize)
{
    float desiredRatio = 
        static_cast<float>(getConfig().resolution.designResolution.x)
        / getConfig().resolution.designResolution.y
    ;

    float currentRatio = 
        static_cast<float>(newScreenSize.x)
        / newScreenSize.y
    ;

    if (std::abs(currentRatio - desiredRatio) > std::numeric_limits<float>::epsilon())
    {
        this->viewport.size = adjustScreenSize(
            newScreenSize, 
            desiredRatio, 
            getConfig().resolution.resizeMode
        );
    }
    else
    {
        this->viewport.size = newScreenSize;
    }

    this->viewport.origin.x = (newScreenSize.x - this->viewport.size.x) / 2;
    this->viewport.origin.y = (newScreenSize.y - this->viewport.size.y) / 2;
}

bool RendererGLFW::doInit(const Config& config)
{
    knM_logDebug("Initilizing GLFW");

    if (!glfwInit())
    {
        knM_logFatal("Can't init GLFW");
        return false;
    }

    GLFWmonitor* monitor = nullptr;

    Sizei screenSize = config.resolution.screenSize;

    if (config.resolution.fullScreen)
    {
        monitor = glfwGetPrimaryMonitor();
     
        if (!monitor)
        {
            knM_logFatal("GLFW didn't return the primary monitor.");
            return false;
        }

        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        screenSize.x = mode->width;
        screenSize.y = mode->height;

        knM_logDebug(
            "Using fullscreen resolution of: " 
            << screenSize.x << ", " << screenSize.y
        );
    }

    if (isUseNoAPI())
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    updateViewport(screenSize);

    this->window = glfwCreateWindow(
        screenSize.x,
        screenSize.y,
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

    if (!config.resolution.vSynced)
    {
        glfwSwapInterval(0);
    }

    glfwSetWindowSizeCallback(getWindow(), onWindowResize);
    glfwSetWindowPosCallback(getWindow(), onWindowResize);

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

Recti RendererGLFW::getViewport() const
{
    return this->viewport;
}

} // namespace graphics
} // namespace kola