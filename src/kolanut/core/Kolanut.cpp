#include "kolanut/core/Kolanut.h"
#include "kolanut/core/Logging.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/scripting/ScriptingEngine.h"
#include "kolanut/events/EventSystem.h"

#include <cassert>

namespace kola {

namespace {
template <typename T, typename C>
bool initSubsystem(
    const char* name,
    std::shared_ptr<T>& inst, 
    std::function<std::shared_ptr<T>(const C&)> createFunc,
    const C& conf
)
{
    inst = createFunc(conf);
    assert(inst);

    if (!inst)
    {
        knM_logFatal("Can't create " << name << ".");
        return false;
    }

    if (!inst->init(conf))
    {
        knM_logFatal("Can't initialize " << name << ".");
        return false;
    }

    return true;
}

} // namespace

bool Kolanut::init(const Config& conf)
{
    knM_logDebug("Initializing Kolanut");

    if (!initSubsystem<events::EventSystem, events::Config>(
        "event system",
        this->eventSystem,
        events::createEventSystem,
        conf.events
    ))
    {
        return false;
    }

    if (!initSubsystem<scripting::ScriptingEngine, scripting::Config>(
        "scripting",
        this->scripting,
        scripting::createScriptingEngine,
        conf.scripting
    ))
    {
        return false;
    }

    if (!initSubsystem<graphics::Renderer, graphics::Config>(
        "renderer",
        this->renderer,
        graphics::createRenderer,
        conf.graphics
    ))
    {
        return false;
    }
    
    return true;
}

void Kolanut::run()
{
    bool isQuit = false;

    this->eventSystem->setQuitCallback([&isQuit] {
        isQuit = true;
    });

    this->scripting->onLoad();

    uint64_t lastFrameTime = this->eventSystem->getTimeMS();
    uint64_t newFrameTime;
    float dt = 1.0f/60.0f; // @todo: 60Hz hardcoded

    while(!isQuit)
    {
        this->eventSystem->poll();
        this->renderer->clear();
        this->scripting->onLoop(dt);
        this->renderer->flip();


        newFrameTime = this->eventSystem->getTimeMS();
        dt = static_cast<float>(newFrameTime - lastFrameTime) / 1000.0f;
        lastFrameTime = newFrameTime;
    }
}

} // namespace kola