/* 
!!!! THIS FILE WAS GENERATED BY THE compile.py PYTHON SCRIPT !!!
!!!! PLEASE RUN THE SCRIPT AGAIN TO UPDATE !!!!
*/

#include "kolanut/scripting/melon/modules/Vector3Module.h"
#include "kolanut/scripting/melon/modules/SourceModule.h"

extern "C" {

TRet vector3ModuleInit(VM* vm)
{
    return sourceModuleInit(vm, "builtin::Vector3.ms", kola::scripting::VECTOR3_MODULE_SOURCE);
}

}

namespace kola {
namespace scripting {

const char* VECTOR3_MODULE_SOURCE = R"##ENDSOURCE##(
let Vector3 = {
    create = func vector3Create |x, y, z| => {
        return { 
            x = number.fromNumber(x ?? 0), 
            y = number.fromNumber(y ?? 0),
            z = number.fromNumber(z ?? 0)
        } @ Vector3;
    },
    [object.symbols.sumOperator] = |other| -> {
        return Vector3.create(this.x + other.x, this.y + other.y, this.z + other.z);
    },
    [object.symbols.subOperator] = |other| -> {
        return Vector3.create(this.x - other.x, this.y - other.y, this.z - other.z);
    },
    [object.symbols.mulOperator] = |other| -> {
        return Vector3.create(this.x * other, this.y * other, this.z * other);
    },
    [object.symbols.divOperator] = |other| -> {
        return Vector3.create(this.x / other, this.y / other, this.z / other);
    },
    [object.symbols.sizeOperator] = -> {
        return math.sqrt(this.x * this.x + this.y * this.y + this.z * this.z);
    },
    [object.symbols.negOperator] = -> {
        return Vector3.create(-this.x, -this.y, -this.z);
    },
    [object.symbols.getIndexOperator] = |idx| -> {
        if (idx == 0) {
            return this.x;
        } else if (idx == 1) {
            return this.y;
        } else if (idx == 2) {
            return this.z;
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
        }

        return idx == 0 || idx == 1 || idx == 2;
    },
    [object.symbols.compareOperator] = |other| -> {
        if (!types.isObject(other)) {
            return 1;
        }
        
        if (this.x == other.x && this.y == other.y && this.z == other.z) {
            return 0;
        }

        return math.round(#this - #other);
    },
    [object.symbols.powOperator] = |other| -> {
        return this.x * other.x + this.y * other.y + this.z * other.z;
    }
};

return Vector3;
)##ENDSOURCE##";

} // namespace scripting
} // namespace kola