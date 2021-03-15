#pragma once

extern "C" {
#include <melon/core/vm.h>
}

namespace kola {
namespace melon {
namespace bindings {

void registerTracyBindings(VM* vm);

} // namespace binding
} // namespace melon
} // namespace kola