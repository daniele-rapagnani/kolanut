#include "kolanut/core/Kolanut.h"
#include "kolanut/core/Logging.h"
#include "kolanut/graphics/Renderer.h"
#include "kolanut/scripting/ScriptingEngine.h"
#include "kolanut/events/EventSystem.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/stats/StatsEngine.h"
#include "kolanut/stats/Stats.h"
#include "kolanut/stats/DummyStats.h"
#include "kolanut/audio/AudioEngine.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <Tracy.hpp>

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

    di::registerType<stats::StatsEngine>(
        [this] () {
            std::shared_ptr<stats::StatsEngine> s = {};

            if (getConfig().enableStats)
            {
                s = std::static_pointer_cast<stats::StatsEngine>(
                    std::make_shared<stats::Stats>()
                );
            }
            else
            {
                s = std::static_pointer_cast<stats::StatsEngine>(
                    std::make_shared<stats::DummyStats>()
                );
            }
            
            stats::StatsEngine::Config c = {};
            c.resultCb = [] (const stats::Stats::Result& r) {
                di::get<scripting::ScriptingEngine>()->onStatsUpdated(r);
            };
            
            s->init(c);

            return s;
        }
    );

    di::registerType<audio::AudioEngine>(
        std::bind(
            initSubsystem<audio::AudioEngine, audio::Config>,
            "audio",
            audio::createAudioEngine,
            this->config.audio
        )
    );

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

    di::create<audio::AudioEngine>();

    return true;
}

void Kolanut::renderFrame()
{
    auto stats = di::get<stats::StatsEngine>();

    stats->addSample(stats::StatsEngine::Measure::AUDIO_PROC, 0);
    stats->startTimeSample(stats::StatsEngine::Measure::FRAME_TIME);
    stats->startTimeSample(stats::StatsEngine::Measure::UPDATE_TIME);

    {
        ZoneScopedN("Events polling")
        getEventSystem()->poll();
    }
    
    {
        ZoneScopedN("Update")
        getStatsEngine()->processEnqueued();
        getScriptingEngine()->onUpdate(this->dt);
    }

    stats->endTimeSample(stats::StatsEngine::Measure::UPDATE_TIME);
    stats->startTimeSample(stats::StatsEngine::Measure::DRAW_TIME);

    getRenderer()->clear();

    {
        ZoneScopedN("OnDraw");
        getScriptingEngine()->onDraw();
    }

    {
        ZoneScopedN("OnDrawUI");
        Vec2f cameraPosTmp = getRenderer()->getCameraPosition();
        float cameraZoomTmp = getRenderer()->getCameraZoom();
        getRenderer()->setCameraPosition({});
        getRenderer()->setCameraZoom(1.0f);
        getScriptingEngine()->onDrawUI();
        getRenderer()->setCameraPosition(cameraPosTmp);
        getRenderer()->setCameraZoom(cameraZoomTmp);
    }
    
    {
        ZoneScopedN("Swap");
        getRenderer()->flip();
    }

    stats->endTimeSample(stats::StatsEngine::Measure::DRAW_TIME);

    uint64_t newFrameTime = getEventSystem()->getTimeMS();

    this->dt = std::min(
        std::max(
            static_cast<float>(newFrameTime - lastFrameTime) / 1000.0f,
            std::numeric_limits<decltype(dt)>::epsilon()
        ),
        (1.0f/60.0f) * 5.0f
    );
    this->lastFrameTime = newFrameTime;

    FrameMark;
    stats->endTimeSample(stats::StatsEngine::Measure::FRAME_TIME);
}

namespace {

void doRenderFrame(void* k)
{
    reinterpret_cast<Kolanut*>(k)->renderFrame();
}

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

    {
        ZoneScopedN("OnLoad")
        getScriptingEngine()->start();
        getScriptingEngine()->onLoad();
    }

    this->lastFrameTime = getEventSystem()->getTimeMS();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(
        doRenderFrame,
        reinterpret_cast<void*>(this),
        -1,
        true
    );
#else
    while(!isQuit)
    {
        renderFrame();
    }
#endif
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

std::shared_ptr<stats::StatsEngine> Kolanut::getStatsEngine() const
{
    return di::get<stats::StatsEngine>(); 
}

} // namespace kola