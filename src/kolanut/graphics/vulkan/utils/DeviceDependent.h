#pragma once

#include <memory>

namespace kola {
namespace graphics {
namespace vulkan {

class Device;

class DeviceDependent
{
public:
    std::shared_ptr<Device> getDevice() const
    { return this->device; }

protected:
    std::shared_ptr<Device> device = {};
};

} // namespace vulkan
} // namespace graphics
} // namespace kola