#include "kolanut/filesystem/zips/ZipsFilesystemEngine.h"
#include "kolanut/core/Logging.h"

#include <cppfs/fs.h>
#include <cppfs/FilePath.h>
#include <cppfs/FileHandle.h>

#include <mz.h>
#include <mz_strm.h>
#include <mz_zip.h>
#include <mz_zip_rw.h>

#include <algorithm>

namespace kola {
namespace filesystem {

bool ZipsFilesystemEngine::init(const Config& config)
{
    if (!FilesFilesystemEngine::init(config))
    {
        return false;
    }

    return loadZipsInDir(getRoot());
}

bool ZipsFilesystemEngine::loadZipsInDir(const std::string& dirPath)
{
    cppfs::FileHandle dir = cppfs::fs::open(dirPath);
    std::vector<std::string> files = dir.listFiles();

    for (const std::string& file : files)
    {
        cppfs::FilePath fpath = cppfs::FilePath(dirPath).resolve(cppfs::FilePath(file));
        cppfs::FileHandle fhpath = cppfs::fs::open(fpath.path());
        
        if (fhpath.isDirectory())
        {
            loadZipsInDir(fpath.path());
        }
        else if (fhpath.isFile() && fpath.extension() == ".nut")
        {
            if (!loadZip(fpath.resolved()))
            {
                return false;
            }
        }
    }

    return true;
}

bool ZipsFilesystemEngine::loadZip(const std::string& path)
{
    std::string dirName = cppfs::FilePath(path).directoryPath();

    void* reader = nullptr;
    int32_t err = MZ_OK;
    mz_zip_reader_create(&reader);

    auto cleanup = [&reader] {
        mz_zip_reader_delete(&reader);
    };

    if (mz_zip_reader_open_file(reader, path.c_str()) != MZ_OK)
    {
        knM_logFatal("Error opening archive '" << path << "': " << err);
        cleanup();
        return false;
    }

    err = mz_zip_reader_goto_first_entry(reader);

    while (err == MZ_OK)
    {
        mz_zip_file* zipFile = nullptr;

        err = mz_zip_reader_entry_get_info(reader, &zipFile);

        if (err != MZ_OK)
        {
            knM_logFatal("Error listing files in archive '" << path << "': " << err);
            break;
        }

        std::string entry(zipFile->filename, zipFile->filename_size);

        ZipEntry ze;
        ze.parentZipPath = path;
        ze.size = zipFile->uncompressed_size;
        ze.directory = mz_zip_reader_entry_is_dir(reader) == MZ_OK;
        ze.filename = entry;
        
        knM_logDebug("Loaded '" << dirName + entry << "' from archive at " << path);

        this->files[dirName + entry] = std::move(ze);

        err = mz_zip_reader_goto_next_entry(reader);
    }

    cleanup();

    if (err == MZ_END_OF_LIST)
    {
        return true;
    }

    if (err != MZ_OK)
    {
        knM_logFatal("Error listing files in archive '" << path << "': " << err);
        return false;
    }

    return true;
}

bool ZipsFilesystemEngine::isFile(const std::string& path)
{
    if (FilesFilesystemEngine::isFile(path))
    {
        return true;
    }

    const ZipEntry* e = getEntry(path);

    if (e)
    {
        return !e->directory;
    }

    return false;
}

size_t ZipsFilesystemEngine::getFileSize(const void* handle)
{
    size_t size = FilesFilesystemEngine::getFileSize(handle);

    if (size > 0)
    {
        return size;
    }

    const ZipHandle* zh = reinterpret_cast<const ZipHandle*>(handle);

    if (zh && zh->entry)
    {
        return zh->entry->size;
    }

    return 0;
}

const ZipsFilesystemEngine::ZipEntry* ZipsFilesystemEngine::getEntry(const std::string& path)
{
    cppfs::FilePath fpath = path;

    auto it = this->files.find(fpath.resolved());

    if (it == this->files.end())
    {
        return nullptr;
    }

    return &it->second;
}

const void* ZipsFilesystemEngine::open(const std::string& file, uint32_t mode)
{
    ZipHandle* zh = new ZipHandle();

    auto handle = 
        reinterpret_cast<const FilesFilesystemEngine::Handle*>(
            FilesFilesystemEngine::open(file, mode)
        )
    ;

    if (handle)
    {
        *zh = *handle;
        return zh;
    }

    const ZipEntry* entry = getEntry(file);

    if (!entry || entry->directory)
    {
        delete zh;
        return nullptr;
    }

    const std::string& archPath = entry->parentZipPath;

    knM_logDebug(
        "Opening file '" << file 
        << "' from archive '" << archPath << "'"
    );

    zh->entry = entry;

    int32_t err = MZ_OK;
    mz_zip_reader_create(&zh->reader);

    auto cleanup = [&zh] {
        mz_zip_reader_delete(&zh->reader);
        delete zh;
    };

    if (mz_zip_reader_open_file(zh->reader, archPath.c_str()) != MZ_OK)
    {
        knM_logError("Error opening archive '" << archPath << "': " << err);
        cleanup();

        return {};
    }

    mz_zip_reader_set_pattern(zh->reader, zh->entry->filename.c_str(), 0);

    if (
        mz_zip_reader_goto_first_entry(zh->reader) != MZ_OK
        || mz_zip_reader_entry_open(zh->reader) != MZ_OK
    )
    {
        knM_logError("Error opening file '" << file << "' in archive '" << archPath << "': " << err);
        cleanup();
        return {};
    }

    return zh;
}

size_t ZipsFilesystemEngine::read(const void* handle, char* buffer, size_t size)
{
    size_t read = FilesFilesystemEngine::read(handle, buffer, size);

    if (read > 0)
    {
        return read;
    }

    const ZipHandle* zh = reinterpret_cast<const ZipHandle*>(handle);

    if (!zh || !zh->entry)
    {
        return 0;
    }

    read = mz_zip_reader_entry_read(zh->reader, buffer, size);

    if (read <= 0)
    {
        return 0;
    }

    return read;
}

void ZipsFilesystemEngine::close(const void* handle)
{
    if (!handle)
    {
        return;
    }

    delete reinterpret_cast<const ZipHandle*>(handle);
}

void ZipsFilesystemEngine::ZipHandle::operator=(const FilesFilesystemEngine::Handle& rhs)
{
    this->file = rhs.file;
    this->input = rhs.input;
    this->output = rhs.output;
    this->mode = rhs.mode;
    this->fh = rhs.fh;
}

ZipsFilesystemEngine::ZipHandle::~ZipHandle()
{
    if (this->reader)
    {
        mz_zip_entry_close(this->reader);
        mz_zip_close(this->reader);

        this->reader = nullptr;
    }
}

} // namespace filesystem
} // namespace kola