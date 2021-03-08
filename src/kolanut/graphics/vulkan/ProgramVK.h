#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/graphics/Program.h"
#include "kolanut/graphics/vulkan/utils/Device.h"
#include "kolanut/graphics/vulkan/utils/DeviceDependent.h"
#include "kolanut/graphics/vulkan/utils/Pipeline2D.h"

#include <string>

namespace kola {
namespace graphics {

class ProgramVK : public Program, public vulkan::DeviceDependent
{
public:
    virtual bool link() override;

    void setDevice(std::shared_ptr<vulkan::Device> device)
    { this->device = device; }

    void setViewportSize(const Sizei& size)
    { this->viewportSize = size; }

    Sizei getViewportSize() const
    { return this->viewportSize; }

    const vulkan::Pipeline2D::Config& getVKPipelineConfig() const
    { return this->pipelineConfig; }

    void setVKPipelineConfig(const vulkan::Pipeline2D::Config& c)
    { this->pipelineConfig = c; }

    std::shared_ptr<vulkan::Pipeline2D> getVKPipeline() const
    { return this->pipeline; }

private:
    vulkan::Pipeline2D::Config pipelineConfig = {};
    std::shared_ptr<vulkan::Pipeline2D> pipeline = {};
    Sizei viewportSize = {};
};

} // namespace graphics
} // namespace kola