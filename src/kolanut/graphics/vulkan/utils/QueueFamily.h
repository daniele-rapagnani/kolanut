#pragma once

#include <cstdint>
#include <cassert>

namespace kola {
namespace graphics {
namespace vulkan {

enum class QueueFamilyType
{
    NONE = 0,
    GRAPHICS,
    TRANSFER,
    PRESENTATION,
    COUNT
};

struct QueueFamily
{
    QueueFamilyType type = QueueFamilyType::NONE;
    uint32_t index = {};

    operator bool() const { return this->type != QueueFamilyType::NONE; }
    operator uint32_t() const { return this->index; }

    bool operator!() const { return !static_cast<bool>(this); }
    
    bool operator==(const QueueFamily& rhs) const
    { return rhs.index == this->index; }
    
    bool operator!=(const QueueFamily& rhs) const
    { return !QueueFamily::operator==(rhs); }
};

} // namespace vulkan
} // namespace graphics
} // namespace kola