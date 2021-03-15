#include "kolanut/core/Logging.h"
#include "kolanut/scripting/melon/ffi/OOP.h"
#include "kolanut/scripting/melon/ffi/Modules.h"
#include "kolanut/scripting/melon/ffi/PopVectors.h"
#include "kolanut/scripting/melon/ffi/PushVectors.h"
#include "kolanut/scripting/melon/ffi/PushContainers.h"
#include "kolanut/scripting/melon/bindings/TilemapFFI.h"

// #define TRACY_ENABLE

#include <Tracy.hpp>
#include <cassert>

namespace kola {
namespace melon {
namespace bindings {

extern "C" {

static TByte beginZone(VM* vm)
{
#ifdef TRACY_ENABLE
    using namespace tracy;

    assert(vm->callStack.size > 1);

    if (vm->callStack.size < 1)
    {
        return 0;
    }

    melM_arg(vm, zoneName, MELON_TYPE_STRING, 0);

    TracyLfqPrepare(tracy::QueueType::ZoneBeginAllocSrcLoc);
    
    CallFrame* cf = melM_stackOffset(&vm->callStack, 1);
    assert(cf->function != NULL);

    uint32_t line = 0;
    const char* source = "no source";
    const char* name = "anonymous function";
    size_t nameSize = strlen(name);
    size_t sourceSize = strlen(source);

    if (cf->function->debug.lines)
    {
        line = cf->function->debug.lines[cf->pc];

        if (cf->function->debug.file != nullptr)
        {
            source = cf->function->debug.file;
            sourceSize = strlen(source);
        }

        if (cf->function->name != nullptr)
        {
            name = melM_strDataFromObj(cf->function->name);
            nameSize = melM_strFromObj(cf->function->name)->len;
        }
    }

    String* melStr = melM_strFromObj(zoneName->pack.obj);

    const auto srcloc = tracy::Profiler::AllocSourceLocation(
        line, 
        source,
        sourceSize,
        name,
        nameSize,
        melStr->string, 
        melStr->len
    );
    
    tracy::MemWrite( &item->zoneBegin.time, tracy::Profiler::GetTime() );
    tracy::MemWrite( &item->zoneBegin.srcloc, srcloc );
    TracyLfqCommit;
#endif

    return 0;
}

static TByte endZone(VM* vm)
{
#ifdef TRACY_ENABLE
    using namespace tracy;

    TracyLfqPrepare(tracy::QueueType::ZoneEnd);
    tracy::MemWrite(&item->zoneEnd.time, tracy::Profiler::GetTime());
    TracyLfqCommit;
#endif

    return 0;
}

static const ModuleFunction funcs[] = {
    // name, args, locals, func
    { "beginZone", 1, 0, &beginZone },
    { "endZone", 0, 0, &endZone },
    { NULL, 0, 0, NULL }
};

}

void registerTracyBindings(VM* vm)
{
    ffi::newSubmodule(vm, "Kolanut", "Tracy", funcs);
}

} // namespace binding
} // namespace melon
} // namespace kola