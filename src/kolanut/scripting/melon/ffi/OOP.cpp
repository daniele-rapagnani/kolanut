#include "kolanut/scripting/melon/ffi/OOP.h"

namespace kola {
namespace melon {
namespace ffi {

Value instanceSymbol;

bool isa(VM* vm, GCItem* obj, const std::string& module, const std::string& className)
{
    Value* classObj = getValueFromModule(*vm, MELON_TYPE_OBJECT, module, className);

    if (!classObj || classObj->type != MELON_TYPE_OBJECT)
    {
        return false;
    }

    GCItem* proto = nullptr;
    GCItem* root = obj;
    
    do {
        proto = melGetPrototypeObject(vm, root);

        if (proto == classObj->pack.obj)
        {
            return true;
        }

        root = proto;
    }
    while (proto);

    return false;
}

} // namespace ffi
} // namespace melon
} // namespace kola