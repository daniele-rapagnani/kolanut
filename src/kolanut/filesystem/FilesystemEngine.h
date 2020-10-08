#pragma once

#include "kolanut/filesystem/Filesystem.h"

#include <vector>
#include <string>

namespace kola {
namespace filesystem {

class FilesystemEngine
{
public:
    virtual bool init(const Config& config) = 0;
    virtual bool getFileContent(const std::string& file, std::vector<char>& content) = 0;
    
    bool getFileContent(const std::string& file, std::string& content);
};

} // namespace filesystem
} // namespace kola