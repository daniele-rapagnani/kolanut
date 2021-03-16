#pragma once

#include "kolanut/filesystem/files/FilesFilesystemEngine.h"

#include <unordered_map>
#include <string>
#include <vector>

namespace kola {
namespace filesystem {

class ZipsFilesystemEngine : public FilesFilesystemEngine
{
private:
    struct ZipEntry
    {
        std::string filename;
        std::string parentZipPath;
        size_t size;
        bool directory = false;
    };

    struct ZipHandle : public FilesFilesystemEngine::Handle
    {
        ~ZipHandle();

        const ZipEntry* entry = {};
        void* reader = {};

        void operator=(const FilesFilesystemEngine::Handle& rhs);
    };

public:
    bool init(const Config& config) override;
    size_t getFileSize(const void* handle) override;
    const void* open(const std::string& file, uint32_t mode) override;
    size_t read(const void* handle, char* buffer, size_t size) override;
    void close(const void* handle) override;
    bool isFile(const std::string& path) override;

protected:
    bool loadZipsInDir(const std::string& dir);
    bool loadZip(const std::string& path);
    const ZipEntry* getEntry(const std::string& path);

private:
    std::unordered_map<std::string, ZipEntry> files;
};

} // namespace filesystem
} // namespace kola