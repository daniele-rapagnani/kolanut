#include "kolanut/scripting/melon/bindings/Bindings.h"
#include "kolanut/scripting/melon/bindings/Texture.h"

namespace kola {
namespace melon {
namespace bindings {

void registerBindings(VM* vm)
{
    registerTextureBindings(vm);
}

} // namespace binding
} // namespace melon
} // namespace kola