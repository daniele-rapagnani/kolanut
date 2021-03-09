#include "kolanut/scripting/melon/bindings/Bindings.h"
#include "kolanut/scripting/melon/bindings/Texture.h"
#include "kolanut/scripting/melon/bindings/Camera.h"
#include "kolanut/scripting/melon/bindings/Font.h"
#include "kolanut/scripting/melon/bindings/Tilemap.h"
#include "kolanut/scripting/melon/bindings/Sound.h"
#include "kolanut/scripting/melon/bindings/SoundInstance.h"
#include "kolanut/scripting/melon/bindings/Transform3D.h"

namespace kola {
namespace melon {
namespace bindings {

void registerBindings(VM* vm)
{
    registerCameraBindings(vm);
    registerTextureBindings(vm);
    registerFontBindings(vm);
    registerTilemapBindings(vm);
    registerSoundBindings(vm);
    registerSoundInstanceBindings(vm);
    registerTransform3DBindings(vm);
}

} // namespace binding
} // namespace melon
} // namespace kola