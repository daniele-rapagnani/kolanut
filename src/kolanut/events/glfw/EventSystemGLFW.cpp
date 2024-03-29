#include "kolanut/events/glfw/EventSystemGLFW.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/Kolanut.h"
#include "kolanut/graphics/ogl/RendererOGL.h"
#include "kolanut/core/DIContainer.h"

#if defined(_ENABLE_VULKAN)
#include "kolanut/graphics/vulkan/RendererVK.h"
#endif

#include <cassert>
#include <unordered_map>

namespace kola {
namespace events {

namespace {

#include "kolanut/events/glfw/KeyCodesMapGLFW.inl"

GLFWwindow* getWindow()
{
    auto rendererType = di::get<Kolanut>()->getConfig().graphics.renderer;

    if (!(
        rendererType == graphics::Engine::OGL
#if defined(_ENABLE_VULKAN)
        || rendererType == graphics::Engine::VULKAN
#endif
    ))
    {
        knM_logFatal("GLFW event system can be used only with the OGL renderer");
        return nullptr;
    }

    std::shared_ptr<graphics::Renderer> renderer = di::get<graphics::Renderer>();
    
#if defined(_ENABLE_VULKAN)
    if (rendererType == graphics::Engine::OGL)
#endif
    {
        auto rendererOgl = std::static_pointer_cast<graphics::RendererOGL>(renderer);
        return rendererOgl->getWindow();
    }

#if defined(_ENABLE_VULKAN)
    auto rendererVk = std::static_pointer_cast<graphics::RendererVK>(renderer);
    return rendererVk->getWindow();
#endif
}

class EventSystemGLFWEX : public EventSystemGLFW
{
public:
    void processKey(int key, int action)
    {
        EventSystemGLFW::processKey(key, action);
    }
};

void onKeyPressed(
    GLFWwindow* window, 
    int key, 
    int scanCode, 
    int action, 
    int modifiers
)
{
    auto es = std::static_pointer_cast<EventSystemGLFWEX>(di::get<EventSystem>());
    es->processKey(key, action);
}

} // namespace

EventSystemGLFW::~EventSystemGLFW()
{
    if (this->window)
    {
        glfwSetKeyCallback(this->window, nullptr);
    }
}

bool EventSystemGLFW::init(const Config& conf)
{
    this->window = getWindow();

    glfwSetKeyCallback(this->window, &onKeyPressed);
    return true;
}

bool EventSystemGLFW::poll()
{
    glfwPollEvents();

    if (glfwWindowShouldClose(this->window))
    {
        if (getQuitCallback())
        {
            getQuitCallback()();
        }
    }

    return true;
}

void EventSystemGLFW::processKey(int key, int action)
{
    if (key == GLFW_KEY_UNKNOWN)
    {
        return;
    }

    assert(GLFW_KOLA_KEYMAP.find(key) != GLFW_KOLA_KEYMAP.end());
    KeyCode keyCode = GLFW_KOLA_KEYMAP[key];

    bool& val = keysPressedMap[static_cast<unsigned int>(keyCode)];
    
    bool prevValue = val;
    bool curValue = action != GLFW_RELEASE;
    val = curValue;

    if (prevValue != curValue && getKeyPressedCallback())
    {
        getKeyPressedCallback()(keyCode, curValue);
    }
}

uint64_t EventSystemGLFW::getTimeMS()
{
    return static_cast<uint64_t>(glfwGetTime() * 1000.0);
}

bool EventSystemGLFW::isKeyPressed(KeyCode key)
{
    return this->keysPressedMap[static_cast<unsigned int>(key)];
}

} // namespace events
} // namespace kola