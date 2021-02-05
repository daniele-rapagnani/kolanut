#include "kolanut/graphics/vulkan/utils/ShaderModule.h"
#include "kolanut/graphics/vulkan/utils/Device.h"

#include <cstdint>

namespace kola {
namespace graphics {
namespace vulkan {

ShaderModule::~ShaderModule()
{
    if (*this)
    {
        vkDestroyShaderModule(this->device->getVkHandle(), getVkHandle(), nullptr);
        this->handle = VK_NULL_HANDLE;
    }
}

bool ShaderModule::init(
    std::shared_ptr<Device> device,
    std::shared_ptr<filesystem::FilesystemEngine> fs,
    const std::string& file
)
{
    std::vector<char> buffer;

    if (!fs->getFileContent(file, buffer))
    {
        knM_logFatal("Can't open shader file: " << file);
        return false;
    }

    return init(device, buffer);
}

bool ShaderModule::init(
    std::shared_ptr<Device> device,
    const std::vector<char>& data
)
{
    VkShaderModuleCreateInfo smci = {};
    smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    smci.codeSize = data.size();
    smci.pCode = reinterpret_cast<const uint32_t*>(&data[0]);

    if (vkCreateShaderModule(device->getVkHandle(), &smci, nullptr, &this->handle) != VK_SUCCESS)
    {
        knM_logFatal("Can't create vertex shader");
        return false;
    }

    this->device = device;

    return true;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola

    