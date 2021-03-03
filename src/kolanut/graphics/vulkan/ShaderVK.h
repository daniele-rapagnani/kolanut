#pragma once

#include "kolanut/graphics/Shader.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"
#include "kolanut/graphics/vulkan/utils/Device.h"
#include "kolanut/graphics/vulkan/utils/ShaderModule.h"

#include <string>
#include <vector>

namespace kola {
namespace graphics {

class ShaderVK : public Shader, public vulkan::DeviceDependent
{
public:
    void setDevice(std::shared_ptr<vulkan::Device> device)
    { this->device = device; }

    std::shared_ptr<vulkan::ShaderModule> getVKShader() const
    { return this->shader; }

protected:
    bool doLoad(const std::vector<char>& data, Type type) override;

private:
    std::shared_ptr<vulkan::ShaderModule> shader = {};
};

} // namespace graphics
} // namespace kola