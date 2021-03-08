#include "kolanut/graphics/vulkan/ProgramVK.h"
#include "kolanut/graphics/vulkan/ShaderVK.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"


namespace kola {
namespace graphics {

bool ProgramVK::link()
{
    auto fragment = getShader(Shader::Type::FRAGMENT);
    auto vertex = getShader(Shader::Type::VERTEX);

    if (!fragment || !vertex)
    {
        return false;
    }

    if (viewportSize.x == 0 || viewportSize.y == 0)
    {
        return false;
    }

    vulkan::Pipeline2D::Config p2dConf = this->pipelineConfig;
    p2dConf.fragmentShader = std::static_pointer_cast<ShaderVK>(fragment)->getVKShader();
    p2dConf.vertexShader = std::static_pointer_cast<ShaderVK>(vertex)->getVKShader();
    p2dConf.renderPass = this->device->getRenderPasses().back();
    p2dConf.viewportWidth = getViewportSize().x;
    p2dConf.viewportHeight = getViewportSize().y;

    this->pipeline = vulkan::make_init<vulkan::Pipeline2D>(this->device, p2dConf);

    if (!this->pipeline)
    {
        return false;
    }

    return true;
}

} // namespace graphics
} // namespace kola