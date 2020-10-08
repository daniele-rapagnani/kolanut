#pragma once

#include "kolanut/core/Types.h"

#include <memory>
#include <string>

namespace kola {
namespace filesystem {

class FilesystemEngine;

enum class Engine
{
    NONE = 0,
    FILES
};

struct Config
{
    Engine renderer = Engine::FILES;
    std::string root = ".";
};

std::shared_ptr<FilesystemEngine> createFilesystemEngine(const Config& conf);

} // namespace filesystem
} // namespace kola