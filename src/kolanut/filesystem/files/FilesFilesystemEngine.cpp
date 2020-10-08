#include "kolanut/filesystem/files/FilesFilesystemEngine.h"
#include "kolanut/core/Logging.h"

#include <fstream>

namespace kola {
namespace filesystem {

namespace {
#ifdef _WIN32
    char PATH_SEP = '\\';
#else
    char PATH_SEP = '/';
#endif
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

    knM_logDebug("Reading file: '" << file << "'");

    std::ifstream fstr;
    std::string path = this->root + file;

    fstr.open(path);

    if (!fstr.is_open())
    {
        knM_logError("Can't open file: '" << file << "'");
        return false;
    }

    fstr.seekg(0, std::ios_base::end);
    size_t size = fstr.tellg();
    fstr.seekg(0, std::ios_base::beg);

    content.resize(size);
    fstr.read(&content[0], size);
    
    bool success = fstr.good();
    fstr.close();

    if (!success)
    {
        knM_logError("Can't read file: '" << file << "'");
    }

    return success;
}

} // namespace filesystem
} // namespace kola