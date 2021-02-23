#include "kolanut/graphics/vulkan/TextureVK.h"
#include "kolanut/graphics/vulkan/RendererVK.h"
#include "kolanut/graphics/vulkan/utils/Texture.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

#include <cstdint>

namespace kola {
namespace graphics {

TextureVK::~TextureVK()
{
}

bool TextureVK::load(const std::string& file)
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

    if (!load(data, w, h))
    {
        stbi_image_free(data);
        knM_logError("Can't create OGL texture from '" << file << "'");
        return false;
    }
    
    stbi_image_free(data);
    
    return true;
}

bool TextureVK::load(unsigned char* data, size_t w, size_t h)
{
    std::shared_ptr<RendererVK> renderer = 
        std::static_pointer_cast<RendererVK>(di::get<graphics::Renderer>())
    ;

    this->texture = vulkan::make_init<vulkan::Texture>(renderer->getDevice(), data, w, h);

    if (!this->texture)
    {
        return false;
    }

    this->size = { w, h };

    return this->texture->transferToGPU(renderer->getGraphicQueue());
}

Sizei TextureVK::getSize()
{
    return this->size;
}

} // namespace graphics
} // namespace kola