#include "kolanut/graphics/TextureSTB.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"

#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

namespace kola {
namespace graphics {

bool TextureSTB::load(const std::string& file)
{
    std::vector<char> buffer;
    
    if (!di::get<filesystem::FilesystemEngine>()->getFileContent(file, buffer))
    {
        knM_logError("Can't read '" << file << "'");
        return false;
    }

    int w, h, n;

    unsigned char* data = stbi_load_from_memory(
        reinterpret_cast<const unsigned char*>(&buffer[0]), 
        buffer.size(), 
        &w, 
        &h, 
        &n, 
        STBI_rgb_alpha
    );
    
    if (!data)
    {
        knM_logError("Can't load '" << file << "': " << stbi_failure_reason());
        return false;
    }

    setDataOwned(false);

    this->size = { w, h };

    if (!load(data, w, h))
    {
        return false;
    }

    if (isDataOwned())
    {
        this->data = data;
    }
    else
    {
        stbi_image_free(data);
    }
    
    return true;
}

Sizei TextureSTB::getSize()
{
    return this->size;
}

} // namespace graphics
} // namespace kola