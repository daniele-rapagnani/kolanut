#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/core/Kolanut.h"

#include <inipp/inipp.h>

#include <iostream>
#include <fstream>

namespace {

void loadBootstrapConf(
	kola::Kolanut::BoostrapConfig& bs, 
	const std::string& file = "bootstrap.ini"
)
{
	inipp::Ini<char> ini;
	std::ifstream is(file);

	if (!is.is_open())
	{
		return;
	}

	ini.parse(is);

	if (!ini.errors.empty())
	{
		knM_logError("Bootstrap ini configuration can't be parsed: ");

		for (const auto& err : ini.errors)
		{
			knM_logError(err);
		}

		return;
	}

	std::string fileEngine = {};

	inipp::get_value(ini.sections["filesystem"], "root", bs.filesystem.root);
	inipp::get_value(ini.sections["filesystem"], "engine", fileEngine);

	inipp::get_value(ini.sections["scripting"], "boot_script", bs.scripting.bootScript);
	inipp::get_value(ini.sections["scripting"], "config_script", bs.scripting.configScript);
	inipp::get_value(ini.sections["scripting"], "scripts_root_dir", bs.scripting.scriptsDir);

	if (fileEngine == "files")
	{
		bs.filesystem.engine = kola::filesystem::Engine::FILES;
	}
}

} // namespace

int main(int argc, char** argv)
{
	kola::di::registerType<kola::Kolanut>([] () -> std::shared_ptr<kola::Kolanut> {
		return std::make_shared<kola::Kolanut>();
	});

	kola::Kolanut::BoostrapConfig bootConf = {};
	loadBootstrapConf(bootConf);

	if (!kola::di::get<kola::Kolanut>()->bootstrap(bootConf))
	{
		knM_logFatal("Can't bootstrap engine");
		return 1;
	}

	kola::Kolanut::Config conf = {};
	kola::di::get<kola::Kolanut>()->loadConfig(conf);

	if (!kola::di::get<kola::Kolanut>()->init(conf))
	{
		knM_logFatal("Can't initialize engine");
		return 2;
	}

	kola::di::get<kola::Kolanut>()->run();

	return 0;
}