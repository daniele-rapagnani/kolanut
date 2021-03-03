#pragma once

#include "kolanut/filesystem/FilesystemEngine.h"

namespace kola {
namespace filesystem {

class FilesFilesystemEngine : public FilesystemEngine
{ 
public:
    bool init(const Config& config) override;
    bool getFileContent(const std::string& file, std::vector<char>& content) override;
    bool isFile(const std::string& path) override;

private:
    std::string root;
};

} // namespace filesystem
} // namespace kola