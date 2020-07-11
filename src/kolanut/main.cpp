#include <iostream>

#include "kolanut/core/Logging.h"
#include "kolanut/core/Kolanut.h"

int main(int argc, char** argv)
{
	kola::Kolanut kn;
	kola::Kolanut::Config conf;
	conf.graphics.windowTitle = "Kolanut";

	if (!kn.init(conf))
	{
		knM_logFatal("Can't initialize engine");
		return 1;
	}

	kn.run();

	return 0;
}