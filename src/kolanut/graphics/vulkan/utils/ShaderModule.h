#pragma once

#include "kolanut/graphics/vulkan/utils/VkHandle.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"
#include "kolanut/filesystem/FilesystemEngine.h"

#include <vector>
#include <cstdint>
#include <string>
#include <memory>

namespace kola {
namespace graphics {
namespace vulkan {

class Device;

class ShaderModule : public VkHandle<VkShaderModule>, public DeviceDependent
{
public:
    ShaderModule() = default;
    ~ShaderModule();

    bool init(
        std::shared_ptr<Device> device,
        std::shared_ptr<filesystem::FilesystemEngine> fs,
        const std::string& file
    );

    bool init(
        std::shared_ptr<Device> device,
        const std::vector<char>& data
    );

    void setEntryPoint(const std::string& ep)
    { this->entryPoint = ep; }

    const std::string& getEntryPoint() const
    { return this->entryPoint; }

private:
    std::string entryPoint = "main";
};

} // namespace vulkan
} // namespace graphics
} // namespace kola