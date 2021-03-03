#pragma once

#include "kolanut/graphics/vulkan/utils/VkHandle.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"

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
    enum class Type
    {
        NONE = 0,
        FRAGMENT,
        VERTEX
    };

    struct Config
    {
        std::string entryPoint = "main";
        Type type = Type::NONE;
    };

public:
    ShaderModule() = default;
    ~ShaderModule();

    bool init(
        const Config& config,
        std::shared_ptr<Device> device,
        const std::string& file,
        const std::vector<char>& data
    );

    const std::string& getEntryPoint() const
    { return this->config.entryPoint; }

    const Config& getConfig() const
    { return this->config; }

    const std::string& getErrors() const
    { return this->errors; }

protected:
    bool compile(const std::vector<char>& source, std::vector<uint32_t>& code);
    bool create(const uint32_t* code, size_t size);

private:
    Config config = {};
    std::string fileName = {};
    std::string errors = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola