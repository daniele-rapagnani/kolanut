
#include "kolanut/core/Logging.h"
#include "kolanut/filesystem/Filesystem.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/filesystem/files/FilesFilesystemEngine.h"

#include <cassert>

namespace kola {
namespace filesystem {

std::shared_ptr<FilesystemEngine> createFilesystemEngine(const Config& conf)
{
    switch(conf.renderer)
    {
        case Engine::FILES:
            return std::make_shared<FilesFilesystemEngine>();
        
        default:
            assert(false);
            knM_logFatal("Unknown renderer: " << static_cast<int>(conf.renderer));
    }

    return nullptr;
}

} // namespace filesystem
} // namespace kola