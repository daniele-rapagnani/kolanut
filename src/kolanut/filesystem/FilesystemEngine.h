#pragma once

#include "kolanut/filesystem/Filesystem.h"

#include <vector>
#include <string>

namespace kola {
namespace filesystem {

class FilesystemEngine
{
public:
    enum OpenMode
    {
        NONE,
        READ = 0x1,
        BINARY = 0x2,
        WRITE = 0x4
    };

public:
    virtual bool init(const Config& config) = 0;
    virtual bool exists(const std::string& file);
    virtual size_t getFileSize(const std::string& file);
    virtual size_t getFileSize(const void* handle) = 0;
    virtual const void* open(const std::string& file, uint32_t mode) = 0;
    virtual size_t read(const void* handle, char* buffer, size_t size) = 0;
    virtual size_t write(const void* handle, char* buffer, size_t size) = 0;
    virtual void close(const void* handle) = 0;
    virtual bool isFile(const std::string& path) = 0;
    virtual std::string resolvePath(const std::string& path) = 0;
    
    bool getFileContent(const std::string& file, std::vector<char>& content);
    bool getFileContent(const std::string& file, std::string& content);
};

} // namespace filesystem
} // namespace kola