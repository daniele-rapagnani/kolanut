#include "kolanut/graphics/vulkan/utils/Pipeline2D.h"
#include "kolanut/graphics/vulkan/utils/Device.h"
#include "kolanut/graphics/Vertex.h"
#include "kolanut/graphics/vulkan/utils/Helpers.h"

#include <cassert>

namespace kola {
namespace graphics {
namespace vulkan {

bool Pipeline2D::init(
    std::shared_ptr<Device> device,
    const Config& config
)
{
    assert(config.fragmentShader);
    assert(config.vertexShader);
    assert(config.viewportWidth > 0);
    assert(config.viewportHeight > 0);
    
    VkPipelineShaderStageCreateInfo pssci[2] = {};
    pssci[0].sType = pssci[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pssci[0].module = config.vertexShader->getVkHandle();
    pssci[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    pssci[0].pName = config.vertexShader->getEntryPoint().c_str();

    pssci[1].module = config.fragmentShader->getVkHandle();
    pssci[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    pssci[1].pName = config.fragmentShader->getEntryPoint().c_str();

    VkPipelineRasterizationStateCreateInfo prsci = {};
    prsci.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    prsci.rasterizerDiscardEnable = VK_FALSE;
    prsci.polygonMode = VK_POLYGON_MODE_FILL;
    prsci.cullMode = VK_CULL_MODE_NONE;
    prsci.frontFace = VK_FRONT_FACE_CLOCKWISE;
    prsci.lineWidth = 1.0f;

    VkPipelineInputAssemblyStateCreateInfo paci = {};
    paci.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    paci.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    paci.primitiveRestartEnable = VK_FALSE;

    VkVertexInputAttributeDescription viad[3] = {};
    viad[0].offset = offsetof(Vertex, pos);
    viad[0].location = 0;
    viad[0].binding = 0;
    viad[0].format = VK_FORMAT_R32G32_SFLOAT;

    viad[1].offset = offsetof(Vertex, uv);
    viad[1].location = 1;
    viad[1].binding = 0;
    viad[1].format = VK_FORMAT_R32G32_SFLOAT;
    
    viad[2].offset = offsetof(Vertex, color);
    viad[2].location = 2;
    viad[2].binding = 0;
    viad[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;

    VkVertexInputBindingDescription vibd = {};
    vibd.binding = 0;
    vibd.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vibd.stride = sizeof(Vertex);

    VkPipelineVertexInputStateCreateInfo pvisci = {};
    pvisci.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pvisci.pVertexAttributeDescriptions = viad;
    pvisci.pVertexBindingDescriptions = &vibd;
    pvisci.vertexAttributeDescriptionCount = 3;
    pvisci.vertexBindingDescriptionCount = 1;

    VkPipelineColorBlendAttachmentState pcbas = {};
    pcbas.blendEnable = VK_TRUE;
    pcbas.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    pcbas.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
    pcbas.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    pcbas.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

    pcbas.colorWriteMask = 
        VK_COLOR_COMPONENT_R_BIT 
        | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT
    ;

    VkPipelineColorBlendStateCreateInfo pcbsci = {};
    pcbsci.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pcbsci.logicOp = VK_LOGIC_OP_COPY;
    pcbsci.logicOpEnable = VK_FALSE;
    pcbsci.attachmentCount = 1;
    pcbsci.pAttachments = &pcbas;

    VkPipelineMultisampleStateCreateInfo pmsci = {};
    pmsci.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pmsci.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkViewport viewport = {};
    viewport.width = config.viewportWidth;
    viewport.height = config.viewportHeight;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    VkRect2D scissoring = {};
    scissoring.extent.width = config.viewportWidth;
    scissoring.extent.height = config.viewportHeight;

    VkPipelineViewportStateCreateInfo pvsci = {};
    pvsci.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pvsci.viewportCount = 1;
    pvsci.pViewports = &viewport;
    pvsci.scissorCount = 1;
    pvsci.pScissors = &scissoring;

    VkDescriptorSetLayoutBinding dslb[3] = {};
    
    dslb[0].binding = 0;
    dslb[0].descriptorCount = 1;
    dslb[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    dslb[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    dslb[1].binding = 1;
    dslb[1].descriptorCount = 1;
    dslb[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    dslb[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    dslb[2].binding = 2;
    dslb[2].descriptorCount = 1;
    dslb[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    dslb[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo dslci = {};
    dslci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dslci.pBindings = dslb;
    dslci.bindingCount = sizeof(dslb) / sizeof(VkDescriptorSetLayoutBinding);

    if (
        vkCreateDescriptorSetLayout(
            device->getVkHandle(), 
            &dslci, 
            nullptr, 
            &this->descriptorSetLayout
        ) != VK_SUCCESS)
    {
        knM_logFatal("Can't create descriptor set layout");
        return false;
    }

    VkPipelineLayoutCreateInfo plci = {};
    plci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    plci.setLayoutCount = 1;
    plci.pSetLayouts = &this->descriptorSetLayout;
    
    if (vkCreatePipelineLayout(device->getVkHandle(), &plci, nullptr, &this->layout) != VK_SUCCESS)
    {
        knM_logFatal("Can' create pipeline layout");
        return false;
    }

    VkGraphicsPipelineCreateInfo gpci = {};
    gpci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    gpci.pStages = pssci;
    gpci.stageCount = 2;
    gpci.renderPass = config.renderPass->getVkHandle();
    gpci.pRasterizationState = &prsci;
    gpci.layout = this->layout;
    gpci.pInputAssemblyState = &paci;
    gpci.pVertexInputState = &pvisci;
    gpci.pColorBlendState = &pcbsci;
    gpci.pMultisampleState = &pmsci;
    gpci.pViewportState = &pvsci;

    if (
        vkCreateGraphicsPipelines(device->getVkHandle(), VK_NULL_HANDLE, 1, &gpci, nullptr, &this->handle)
        != VK_SUCCESS
    )
    {
        knM_logFatal("Can't create pipeline");
        return false;
    }

    this->device = device;
    this->config = config;

    return true;
}

} // namespace vulkan
} // namespace graphics
} // namespace kola