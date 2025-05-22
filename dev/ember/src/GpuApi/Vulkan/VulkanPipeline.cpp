#include "GpuApi/Vulkan/VulkanPipeline.h"
#include "Renderer/Vulkan/VertexVulkan.h"

namespace ember {

    void VulkanGraphicsPipeline::SetVertexLayoutInterleaved(const VertexBufferInfo& vbInfo) {
        vertexBindingDescs.resize(1);
        vertexBindingDescs[0].binding = 0;
        vertexBindingDescs[0].stride = vbInfo.vertexStride;
        vertexBindingDescs[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        vertexAttribDescs = VertexAttribLayoutToVulkanAttribDescription(VertexPC::attributes);
    }

    void VulkanGraphicsPipeline::SetPrimitiveTopology(VkPrimitiveTopology topology) {
        this->topology = topology;
    }

    void VulkanGraphicsPipeline::SetViewport(VkViewport viewport) {
        this->viewport = viewport;
    }
    void VulkanGraphicsPipeline::SetScissors(VkRect2D scissors) {
        this->scissors = scissors;
    }

    void VulkanGraphicsPipeline::SetPolygonMode(VkPolygonMode vulkanPolygonMode) {
        this->polygonMode = vulkanPolygonMode;
    }
    void VulkanGraphicsPipeline::SetCullMode(VkCullModeFlagBits vulkanCullMode) {
        this->cullMode = vulkanCullMode;
    }
    void VulkanGraphicsPipeline::SetFrontFace(VkFrontFace vulkanFrontFace) {
        this->frontFace = vulkanFrontFace;
    }
    void VulkanGraphicsPipeline::SetLineWidth(float lineWidth) {
        this->lineWidth = lineWidth;
    }

    void VulkanGraphicsPipeline::EnableMultisampling(VkSampleCountFlagBits vulkanSampleCount) {
        this->samples = vulkanSampleCount;
        this->multisamplingEnabled = true;
    }
    void VulkanGraphicsPipeline::DisableMultisampling() {
        this->samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
        this->multisamplingEnabled = false;
    }

    void VulkanGraphicsPipeline::AddDynamicState(VkDynamicState dynamicState) {
        dynamicStates.push_back(dynamicState);
    }

    VkPipelineVertexInputStateCreateInfo VulkanGraphicsPipeline::CreateVulkanVertexInputStateInfo() const {
        VkPipelineVertexInputStateCreateInfo inputStateInfo{};
        inputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        inputStateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDescs.size());
        inputStateInfo.pVertexBindingDescriptions = vertexBindingDescs.data();
        inputStateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttribDescs.size());
        inputStateInfo.pVertexAttributeDescriptions = vertexAttribDescs.data();
        return inputStateInfo;
    }
    VkPipelineInputAssemblyStateCreateInfo VulkanGraphicsPipeline::CreateVulkanInputAssemblyStateInfo() const {
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
        inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyInfo.topology = topology;
        inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
        return inputAssemblyInfo;
    }
    VkPipelineViewportStateCreateInfo VulkanGraphicsPipeline::CreateVulkanViewportAndScissorsStateInfo() const {
        VkPipelineViewportStateCreateInfo viewportStateInfo{};
        viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateInfo.viewportCount = 1;
        viewportStateInfo.scissorCount = 1;
        viewportStateInfo.pViewports = &viewport;
        viewportStateInfo.pScissors = &scissors;
        return viewportStateInfo;
    }
    VkPipelineRasterizationStateCreateInfo VulkanGraphicsPipeline::CreateVulkanRasterizationStateInfo() const {
        VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
        rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationInfo.depthClampEnable = VK_FALSE;
        rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationInfo.polygonMode = polygonMode;
        rasterizationInfo.lineWidth = lineWidth;
        rasterizationInfo.cullMode = cullMode;
        rasterizationInfo.frontFace = frontFace;
        rasterizationInfo.depthBiasEnable = VK_FALSE;
        rasterizationInfo.depthBiasClamp = 0.0f;
        rasterizationInfo.depthBiasConstantFactor = 0.0f;
        rasterizationInfo.depthBiasSlopeFactor = 0.0f;
        return rasterizationInfo;
    }
    VkPipelineMultisampleStateCreateInfo VulkanGraphicsPipeline::CreateVulkanMultisampleStateInfo() const {
        VkPipelineMultisampleStateCreateInfo multisampleStateInfo{};
        multisampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateInfo.sampleShadingEnable = multisamplingEnabled ? VK_TRUE : VK_FALSE;
        multisampleStateInfo.rasterizationSamples = samples;
        multisampleStateInfo.minSampleShading = 1.0f;
        multisampleStateInfo.pSampleMask = nullptr;
        multisampleStateInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleStateInfo.alphaToOneEnable = VK_FALSE;

        return multisampleStateInfo;
    }
    VkPipelineColorBlendStateCreateInfo VulkanGraphicsPipeline::CreateVulkanBlendingStateInfo() const {
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        // colorBlending.attachmentCount = static_cast<uint32_t>(blendingAttachmentStates.size());
        // colorBlending.pAttachments = blendingAttachmentStates.data();
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        return colorBlending;
    }
    VkPipelineDynamicStateCreateInfo VulkanGraphicsPipeline::CreateVulkanDynamicStateInfo() const {
        VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicStateInfo.pDynamicStates = dynamicStates.data();
        return dynamicStateInfo;
    }

}