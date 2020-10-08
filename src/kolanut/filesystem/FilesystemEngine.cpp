#include "kolanut/filesystem/FilesystemEngine.h"

namespace kola {
namespace filesystem {

bool FilesystemEngine::getFileContent(const std::string& file, std::string& content)
{
    std::vector<char> buffer;
    
    if (!getFileContent(file, buffer))
    {
        return false;
    }

    content = std::string(buffer.begin(), buffer.end());
    return true;
}

} // namespace filesystem
} // namespace kola