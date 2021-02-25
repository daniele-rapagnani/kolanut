#include "kolanut/core/Kolanut.h"
#include "kolanut/core/Logging.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/scripting/ScriptingEngine.h"
#include "kolanut/events/EventSystem.h"
#include "kolanut/filesystem/FilesystemEngine.h"

#include <cassert>
#include <limits>

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

bool Kolanut::bootstrap(const BoostrapConfig& conf)
{
    knM_logDebug("Bootstrapping Kolanut");
    this->bootstrapConfig = conf;

    di::registerType<filesystem::FilesystemEngine>(
        std::bind(
            initSubsystem<filesystem::FilesystemEngine, filesystem::Config>,
            "filesystem",
            filesystem::createFilesystemEngine,
            this->bootstrapConfig.filesystem
        )
    );

    di::registerType<scripting::ScriptingEngine>(
        std::bind(
            initSubsystem<scripting::ScriptingEngine, scripting::Config>,
            "scripting",
            scripting::createScriptingEngine,
            this->bootstrapConfig.scripting
        )
    );

    return true;
}

bool Kolanut::loadConfig(Config& conf)
{
    return di::get<scripting::ScriptingEngine>()->loadConfig(conf);
}

bool Kolanut::init(const Config& conf)
{
    knM_logDebug("Initializing Kolanut");
    this->config = conf;

    di::registerType<graphics::Renderer>(
        std::bind(
            initSubsystem<graphics::Renderer, graphics::Config>,
            "graphics",
            graphics::createRenderer,
            this->config.graphics
        )
    );

    di::registerType<events::EventSystem>(
        std::bind(
            initSubsystem<events::EventSystem, events::Config>,
            "event system",
            events::createEventSystem,
            this->config.events
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

    getScriptingEngine()->start();
    getScriptingEngine()->onLoad();

    uint64_t lastFrameTime = getEventSystem()->getTimeMS();
    uint64_t newFrameTime;
    float dt = 1.0f/60.0f; // @todo: 60Hz hardcoded

    Vec2f cameraPosTmp = {};
    float cameraZoomTmp;

    while(!isQuit)
    {
        getEventSystem()->poll();
        getScriptingEngine()->onUpdate(dt);

        getRenderer()->clear();
        getScriptingEngine()->onDraw();

        cameraPosTmp = getRenderer()->getCameraPosition();
        cameraZoomTmp = getRenderer()->getCameraZoom();
        getRenderer()->setCameraPosition({});
        getRenderer()->setCameraZoom(1.0f);
        getScriptingEngine()->onDrawUI();
        getRenderer()->setCameraPosition(cameraPosTmp);
        getRenderer()->setCameraZoom(cameraZoomTmp);
        
        getRenderer()->flip();

        newFrameTime = getEventSystem()->getTimeMS();
        dt = std::min(
            std::max(
                static_cast<float>(newFrameTime - lastFrameTime) / 1000.0f,
                std::numeric_limits<decltype(dt)>::epsilon()
            ),
            (1.0f/60.0f) * 5.0f
        );
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