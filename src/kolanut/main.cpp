#include <iostream>

#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/core/Kolanut.h"

int main(int argc, char** argv)
{
	kola::di::registerType<kola::Kolanut>([] () -> std::shared_ptr<kola::Kolanut> {
		return std::make_shared<kola::Kolanut>();
	});

	kola::Kolanut::BoostrapConfig bootConf = {};

	if (!kola::di::get<kola::Kolanut>()->bootstrap(bootConf))
	{
		knM_logFatal("Can't bootstrap engine");
		return 1;
	}

	kola::Kolanut::Config conf = {};
	kola::di::get<kola::Kolanut>()->loadConfig(conf);

	// kola::Kolanut::Config conf;
	// conf.graphics.windowTitle = "Kolanut";
	// conf.graphics.resolution.designResolution = conf.graphics.resolution.screenSize / 2;
	// conf.graphics.renderer = kola::graphics::Engine::VULKAN;
	// conf.events.eventSystem = kola::events::Engine::GLFW;

	if (!kola::di::get<kola::Kolanut>()->init(conf))
	{
		knM_logFatal("Can't initialize engine");
		return 2;
	}

	kola::di::get<kola::Kolanut>()->run();

	return 0;
}