#pragma once

#include "kolanut/scripting/ScriptingEngine.h"

extern "C" {
#include <melon/core/vm.h>
}

namespace kola {
namespace scripting {

class ScriptingEngineMelon : public ScriptingEngine
{
public:
    bool init(const Config& config) override;

private:
    VMConfig vmConfig;
    VM vm;
};

} // namespace scripting
} // namespace kola