/* 
!!!! THIS FILE WAS GENERATED BY THE compile.py PYTHON SCRIPT !!!
!!!! PLEASE RUN THE SCRIPT AGAIN TO UPDATE !!!!
*/

#include "kolanut/scripting/melon/modules/SpriteModule.h"
#include "kolanut/scripting/melon/modules/SourceModule.h"

extern "C" {

TRet spriteModuleInit(VM* vm)
{
    return sourceModuleInit(vm, "builtin::Sprite.ms", kola::scripting::SPRITE_MODULE_SOURCE);
}

}

namespace kola {
namespace scripting {

const char* SPRITE_MODULE_SOURCE = R"##ENDSOURCE##(
let Sprite = {
    create = |name| => {
        let s = { 
            pos = Vector.create(0.0, 0.0),
            angle = 0.0,
            scale = Vector.create(1.0, 1.0),
            texture = Kolanut.loadSprite(name)
        } @ Sprite;

        if (s.texture) {
            s.size = s.texture->getSize();
            s.rect = Rect.create(0.0, 0.0, s.size.x, s.size.y);
        }

        return s;
    },

    draw = -> {
        // this.texture->draw(
        //     this.pos.x, this.pos.y,
        //     this.angle,
        //     this.scale.x, this.scale.y
        // );
    }
};

return Sprite;
)##ENDSOURCE##";

} // namespace scripting
} // namespace kola