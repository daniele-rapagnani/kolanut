#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/graphics/Tilemap.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/PushPrimitives.h"

extern "C" {
#include <melon/core/vm.h>
#include <melon/core/tstring.h>
#include <melon/modules/modules.h>
}

namespace kola {
namespace melon {
namespace ffi {

template <>
class Push<const Tmx::Object*>
{
public:
    static TByte push(VM* vm, const Tmx::Object* const & val)
    {
        if (!val)
        {
            melM_vstackPushNull(&vm->stack);
            return 1;
        }

        GCItem* obj = melNewObject(vm);
        melM_vstackPushGCItem(&vm->stack, obj);

        ffi::setInstanceField(vm, obj, "name", val->GetName());
        ffi::setInstanceField(vm, obj, "type", val->GetType());
        ffi::setInstanceField(vm, obj, "x", val->GetX());
        ffi::setInstanceField(vm, obj, "y", val->GetY());
        ffi::setInstanceField(vm, obj, "w", val->GetWidth());
        ffi::setInstanceField(vm, obj, "h", val->GetHeight());
        
        return 1;
    }
};

template <>
class Push<Tmx::Object*>
{
public:
    static TByte push(VM* vm, const Tmx::Object* const & val)
    {
        return Push<const Tmx::Object*>::push(vm, val);
    }
};

template <>
class Push<const graphics::Tilemap::Tile*>
{
public:
    static TByte push(VM* vm, const graphics::Tilemap::Tile* const& val)
    {
        if (!val || !val->tile)
        {
            melM_vstackPushNull(&vm->stack);
            return 1;
        }

        GCItem* obj = melNewObject(vm);
        melM_vstackPushGCItem(&vm->stack, obj);

        ffi::setInstanceField(vm, obj, "gid", val->tile->gid);
        ffi::setInstanceField(vm, obj, "id", val->tile->id);
        ffi::setInstanceField(vm, obj, "position", val->position);
        ffi::setInstanceField(vm, obj, "rect", val->rect);

        return 1;
    }
};

template <>
class Push<graphics::Tilemap::Tile*>
{
public:
    static TByte push(VM* vm, const graphics::Tilemap::Tile* const & val)
    {
        return Push<const graphics::Tilemap::Tile*>::push(vm, val);
    }
};

} // namespace ffi
} // namespace melon
} // namespace kola