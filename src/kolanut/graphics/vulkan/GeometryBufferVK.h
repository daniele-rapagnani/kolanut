#pragma once

#include "kolanut/graphics/GeometryBuffer.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"
#include "kolanut/graphics/vulkan/utils/Buffer.h"
#include "kolanut/graphics/vulkan/utils/CommandBuffer.h"

#include <vulkan/vulkan.h>

#include <vector>
#include <string>
#include <functional>
#include <cstdint>

namespace kola {
namespace graphics {

class GeometryBufferVK : public GeometryBuffer, public vulkan::DeviceDependent
{
public:
    void setDevice(std::shared_ptr<vulkan::Device> device)
    { this->device = device; }

    bool bind(Handle h, std::shared_ptr<vulkan::CommandBuffer> commandBuf);

protected:
    bool createBuffer() override;
    bool copy(const Vertex* vertices, size_t base, size_t size, size_t* realSize) override;

private:
    std::shared_ptr<vulkan::Buffer> buffer = {};
};

} // namespace graphics
} // namespace kola