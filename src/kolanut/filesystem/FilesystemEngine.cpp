#include "kolanut/core/Logging.h"
#include "kolanut/filesystem/FilesystemEngine.h"

namespace kola {
namespace filesystem {

bool FilesystemEngine::getFileContent(const std::string& file, std::vector<char>& content)
{
    if (file.empty())
    {
        return false;
    }

    const void* handle = open(file, OpenMode::READ | OpenMode::BINARY);

    if (!handle)
    {
        return 0;
    }

    size_t size = getFileSize(handle);

    if (size == 0)
    {
        return 0;
    }

    content.resize(size);
    size_t bytesRead = read(handle, &content[0], size);
    
    bool success = true;

    if (bytesRead == 0)
    {
        knM_logError("Can't read file: '" << file << "'");
        success = false;
    }

    close(handle);

    return success;
}

bool FilesystemEngine::exists(const std::string& file)
{
    const void* handle = open(file, OpenMode::READ);
    return handle;
}

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

size_t FilesystemEngine::getFileSize(const std::string& file)
{
    const void* handle = open(file, OpenMode::READ);

    if (!handle)
    {
        return 0;
    }

    return getFileSize(handle);
}

} // namespace filesystem
} // namespace kola