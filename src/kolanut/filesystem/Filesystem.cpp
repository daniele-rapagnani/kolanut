
#include "kolanut/core/Logging.h"
#include "kolanut/filesystem/Filesystem.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/filesystem/files/FilesFilesystemEngine.h"
#include "kolanut/filesystem/zips/ZipsFilesystemEngine.h"

#include <cassert>

namespace kola {
namespace filesystem {

std::shared_ptr<FilesystemEngine> createFilesystemEngine(const Config& conf)
{
    switch(conf.engine)
    {
        case Engine::FILES:
            return std::make_shared<FilesFilesystemEngine>();

        case Engine::ZIPS:
            return std::make_shared<ZipsFilesystemEngine>();
        
        default:
            assert(false);
            knM_logFatal("Unknown renderer: " << static_cast<int>(conf.engine));
    }

    return nullptr;
}

} // namespace filesystem
} // namespace kola