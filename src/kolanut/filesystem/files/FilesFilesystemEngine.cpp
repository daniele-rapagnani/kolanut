#include "kolanut/filesystem/files/FilesFilesystemEngine.h"
#include "kolanut/core/Logging.h"

#include <cppfs/fs.h>
#include <cppfs/FilePath.h>
#include <cppfs/FileHandle.h>

extern "C" {
#include "kolanut/libs/melonscript-vm/src/melon/core/utils.h"
}

namespace kola {
namespace filesystem {

namespace {
const char PATH_SEP = '/';
}

bool FilesFilesystemEngine::init(const Config& config)
{
    this->root = config.root;
    
    if (this->root.empty())
    {
        this->root = ".";
    }

    if (this->root[this->root.size() - 1] != PATH_SEP)
    {
        this->root += PATH_SEP;
    }

    return true;
}

bool FilesFilesystemEngine::getFileContent(const std::string& file, std::vector<char>& content)
{
    if (file.empty())
    {
        return false;
    }
    
    cppfs::FilePath path = this->root + file;

    knM_logDebug("Reading file: '" << path.toNative() << "'");

    cppfs::FileHandle fh = cppfs::fs::open(path.path());

    if (!fh.isFile())
    {
        knM_logError("Can't open file: '" << path.toNative() << "'");
        return false;
    }

    size_t size = fh.size();
    auto is = fh.createInputStream(std::ios::binary);

    content.resize(size);
    is->read(&content[0], size);
    
    bool success = !is->fail();

    if (!success)
    {
        knM_logError("Can't read file: '" << path.toNative() << "'");
    }

    return success;
}

} // namespace filesystem
} // namespace kola