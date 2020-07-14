#include "kolanut/core/Kolanut.h"
#include "kolanut/core/Logging.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/scripting/ScriptingEngine.h"
#include "kolanut/events/EventSystem.h"

#include <cassert>

namespace kola {

namespace {
template <typename T, typename C>
std::shared_ptr<T> initSubsystem(
    const char* name,
    std::function<std::shared_ptr<T>(const C&)> createFunc,
    const C& conf
)
{
    std::shared_ptr<T> inst = createFunc(conf);
    assert(inst);

    if (!inst)
    {
        knM_logFatal("Can't create " << name << ".");
        return nullptr;
    }

    if (!inst->init(conf))
    {
        knM_logFatal("Can't initialize " << name << ".");
        return nullptr;
    }

    return inst;
}

} // namespace

bool Kolanut::init(const Config& conf)
{
    knM_logDebug("Initializing Kolanut");

    di::registerType<scripting::ScriptingEngine>(
        std::bind(
            initSubsystem<scripting::ScriptingEngine, scripting::Config>,
            "scripting",
            scripting::createScriptingEngine,
            conf.scripting
        )
    );

    di::registerType<events::EventSystem>(
        std::bind(
            initSubsystem<events::EventSystem, events::Config>,
            "event system",
            events::createEventSystem,
            conf.events
        )
    );

    di::registerType<graphics::Renderer>(
        std::bind(
            initSubsystem<graphics::Renderer, graphics::Config>,
            "graphics",
            graphics::createRenderer,
            conf.graphics
        )
    );

    return true;
}

void Kolanut::run()
{
    bool isQuit = false;

    getEventSystem()->setQuitCallback([&isQuit, this] {
        if (this->getScriptingEngine()->onQuit())
        {
            isQuit = true;
        }
    });

    getEventSystem()->setKeyPressedCallback(
        [this] (events::KeyCode key, bool pressed) {
            this->getScriptingEngine()->onKeyPressed(key, pressed);
        }
    );

    getScriptingEngine()->onLoad();

    uint64_t lastFrameTime = getEventSystem()->getTimeMS();
    uint64_t newFrameTime;
    float dt = 1.0f/60.0f; // @todo: 60Hz hardcoded

    while(!isQuit)
    {
        getEventSystem()->poll();
        getScriptingEngine()->onUpdate(dt);

        getRenderer()->clear();
        getScriptingEngine()->onDraw();
        getRenderer()->flip();

        newFrameTime = getEventSystem()->getTimeMS();
        dt = static_cast<float>(newFrameTime - lastFrameTime) / 1000.0f;
        lastFrameTime = newFrameTime;
    }
}

std::shared_ptr<events::EventSystem> Kolanut::getEventSystem() const
{ 
    return di::get<events::EventSystem>(); 
}

std::shared_ptr<scripting::ScriptingEngine> Kolanut::getScriptingEngine() const
{ 
    return di::get<scripting::ScriptingEngine>(); 
}

std::shared_ptr<graphics::Renderer> Kolanut::getRenderer() const
{ 
    return di::get<graphics::Renderer>(); 
}

} // namespace kola