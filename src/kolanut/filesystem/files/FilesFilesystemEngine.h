#pragma once

#include "kolanut/filesystem/FilesystemEngine.h"

#include <cppfs/FilePath.h>
#include <cppfs/FileHandle.h>

#include <fstream>
#include <memory>

namespace kola {
namespace filesystem {

class FilesFilesystemEngine : public FilesystemEngine
{ 
protected:
    struct Handle
    {
        ~Handle();

        std::string file = {};

        cppfs::FileHandle fh = {};

        std::shared_ptr<std::istream> input = {};
        std::shared_ptr<std::ostream> output = {};
        uint32_t mode = OpenMode::NONE;

        bool isWriteMode() const
        { return (this->mode & OpenMode::WRITE) == OpenMode::WRITE; }

        bool isReadMode() const
        { return (this->mode & OpenMode::READ) == OpenMode::READ; }

        operator bool() const
        { return this->input || this->output; }
    };

public:
    virtual bool init(const Config& config) override;
    virtual size_t getFileSize(const void* handle) override;
    virtual const void* open(const std::string& file, uint32_t mode) override;
    virtual size_t read(const void* handle, char* buffer, size_t size) override;
    virtual size_t write(const void* handle, char* buffer, size_t size) override;
    virtual void close(const void* handle) override;
    virtual bool isFile(const std::string& path) override;
    virtual std::string resolvePath(const std::string& path) override;

protected:
    cppfs::FilePath getPath(const std::string& path) const;

    const std::string& getRoot() const
    { return root; }

private:
    std::string root;
};

} // namespace filesystem
} // namespace kola