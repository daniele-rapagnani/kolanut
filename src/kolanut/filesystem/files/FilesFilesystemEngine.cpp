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

bool FilesFilesystemEngine::init(const Config& config)
{
    std::string root = config.root;
    
    if (root.empty())
    {
        root = ".";
    }

    if (root[root.size() - 1] != '/')
    {
        root += '/';
    }

    cppfs::FilePath rootPath = root;
    cppfs::FileHandle rootH = cppfs::fs::open(rootPath.path());

    if (!rootH.isDirectory())
    {
        return false;
    }

    this->root = root;

    return true;
}

bool FilesFilesystemEngine::isFile(const std::string& path)
{
    cppfs::FilePath fpath = getPath(path);
    cppfs::FileHandle fh = cppfs::fs::open(fpath.path());

    return fh.isFile();
}

size_t FilesFilesystemEngine::getFileSize(const void* handle)
{
    const Handle* h = reinterpret_cast<const Handle*>(handle);

    if (!h)
    {
        return 0;
    }

    return h->fh.size();
}

cppfs::FilePath FilesFilesystemEngine::getPath(const std::string& path) const
{
    cppfs::FilePath fpath = path;

    if (fpath.isRelative())
    {
        fpath = this->root + path;
    }

    return fpath;
}

std::string FilesFilesystemEngine::stripRoot(const std::string& path) const
{
    if (
        path.size() > getRoot().size() &&
        path.substr(0, getRoot().size()) == getRoot()
    )
    {
        return path.substr(getRoot().size());
    }

    return path;
}

const void* FilesFilesystemEngine::open(const std::string& file, uint32_t mode)
{
    cppfs::FilePath fpath = getPath(file);
    cppfs::FileHandle handle = cppfs::fs::open(fpath.path());

    knM_logDebug("Opening file '" << fpath.toNative() << "' from filesystem");

    std::ios::openmode stdMode = static_cast<std::ios::openmode>(
        ((mode & OpenMode::BINARY) == OpenMode::BINARY) ?
        std::ios::binary : 0
    );

    Handle* fhandle = new Handle();
    fhandle->mode = mode;
    fhandle->file = file;
    fhandle->fh = handle;
    
    if (fhandle->isWriteMode())
    {
        fhandle->output = handle.createOutputStream(stdMode);
    }
    else if (fhandle->isReadMode())
    {
        fhandle->input = handle.createInputStream(stdMode);
    }

    if (!*fhandle)
    {
        delete fhandle;
        fhandle = nullptr;
    }

    return fhandle;
}

size_t FilesFilesystemEngine::read(const void* handle, char* buffer, size_t size)
{
    const Handle* h = reinterpret_cast<const Handle*>(handle);

    if (!h || !*h || !h->isReadMode())
    {
        return 0;
    }

    if (!h->input->good())
    {
        return 0;
    }

    h->input->read(buffer, size);
    return h->input->gcount();
}

void FilesFilesystemEngine::close(const void* handle)
{
    const Handle* h = reinterpret_cast<const Handle*>(handle);

    if (!h)
    {
        return;
    }

    delete h;
}

std::string FilesFilesystemEngine::resolvePath(const std::string& path)
{
    cppfs::FilePath fpath = path;
    return fpath.resolved();
}

} // namespace filesystem
} // namespace kola