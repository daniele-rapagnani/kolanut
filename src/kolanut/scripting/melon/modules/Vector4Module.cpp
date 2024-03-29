/* 
!!!! THIS FILE WAS GENERATED BY THE compile.py PYTHON SCRIPT !!!
!!!! PLEASE RUN THE SCRIPT AGAIN TO UPDATE !!!!
*/

#include "kolanut/scripting/melon/modules/Vector4Module.h"
#include "kolanut/scripting/melon/modules/SourceModule.h"

extern "C" {

TRet vector4ModuleInit(VM* vm)
{
    return sourceModuleInit(vm, "builtin::Vector4.ms", kola::scripting::VECTOR4_MODULE_SOURCE);
}

}

namespace kola {
namespace scripting {

const char* VECTOR4_MODULE_SOURCE = R"##ENDSOURCE##(
let Vector4 = {
    create = func vector4Create |x, y, z, w| => {
        return { 
            x = number.fromNumber(x ?? 0), 
            y = number.fromNumber(y ?? 0),
            z = number.fromNumber(z ?? 0),
            w = number.fromNumber(w ?? 0)
        } @ Vector4;
    },
    [object.symbols.sumOperator] = |other| -> {
        return Vector4.create(this.x + other.x, this.y + other.y, this.z + other.z, this.w + other.w);
    },
    [object.symbols.subOperator] = |other| -> {
        return Vector4.create(this.x - other.x, this.y - other.y, this.z - other.z, this.w - other.w);
    },
    [object.symbols.mulOperator] = |other| -> {
        return Vector4.create(this.x * other, this.y * other, this.z * other, this.w * other);
    },
    [object.symbols.divOperator] = |other| -> {
        return Vector4.create(this.x / other, this.y / other, this.z / other, this.w / other);
    },
    [object.symbols.sizeOperator] = -> {
        return math.sqrt(this.x * this.x + this.y * this.y + this.z * this.z + this.w * this.w);
    },
    [object.symbols.negOperator] = -> {
        return Vector4.create(-this.x, -this.y, -this.z, -this.w);
    },
    [object.symbols.getIndexOperator] = |idx| -> {
        if (idx == 0) {
            return this.x;
        } else if (idx == 1) {
            return this.y;
        } else if (idx == 2) {
            return this.z;
        } else if (idx == 3) {
            return this.w;
        }

        return false;
    },
    [object.symbols.setIndexOperator] = |idx, val| -> {
        if (!types.isInteger(idx)) {
            return false;
        }

        if (idx == 0) {
            this.x = val;
        } else if (idx == 1) {
            this.y = val;
        } else if (idx == 2) {
            this.z = val;
        } else if (idx == 3) {
            this.w = val;
        }

        return idx == 0 || idx == 1 || idx == 2 || idx == 3;
    },
    [object.symbols.compareOperator] = |other| -> {
        if (!types.isObject(other)) {
            return 1;
        }
        
        if (this.x == other.x && this.y == other.y && this.z == other.z && this.w == other.w) {
            return 0;
        }

        return math.round(#this - #other);
    },
    [object.symbols.powOperator] = |other| -> {
        return this.x * other.x + this.y * other.y + this.z * other.z + this.w * other.w;
    }
};

return Vector4;
)##ENDSOURCE##";

} // namespace scripting
} // namespace kola