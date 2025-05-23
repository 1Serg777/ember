#include "GpuApi/Vulkan/VulkanPipeline.h"
#include "GpuApi/Vulkan/VulkanVertex.h"

#include <stdexcept>

namespace ember {

    void VulkanGraphicsPipeline::CreatePipeline(VkDevice device) {
        bool vertexShaderExists = vertexShaderModule.has_value();
        bool fragmentShaderExists = fragmentShaderModule.has_value();
        if (!vertexShaderExists || !fragmentShaderExists) {
            throw std::runtime_error{ "Mandatory Vertex and/or Fragment shader(s) is (are) not present!" };
        }
        VkPipelineShaderStageCreateInfo shaderStages[] = {
            CreateVertexShaderStageCreateInfo(),
            CreateFragmentShaderStageCreateInfo()
        };
        VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = CreateVulkanVertexInputStateInfo();
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = CreateVulkanInputAssemblyStateInfo();
        VkPipelineViewportStateCreateInfo viewportStateInfo = CreateVulkanViewportStateInfo();
        VkPipelineRasterizationStateCreateInfo rasterizationStateInfo = CreateVulkanRasterizationStateInfo();
        VkPipelineMultisampleStateCreateInfo multisampleStateInfo = CreateVulkanMultisampleStateInfo();
        VkPipelineColorBlendStateCreateInfo blendingStateInfo = CreateVulkanBlendingStateInfo();
        VkPipelineDynamicStateCreateInfo dynamicStateInfo = CreateVulkanDynamicStateInfo();

        VkGraphicsPipelineCreateInfo graphicsPipelineInfo{};
        graphicsPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        graphicsPipelineInfo.stageCount = 2;
        graphicsPipelineInfo.pStages = shaderStages;
        graphicsPipelineInfo.pVertexInputState = &vertexInputStateInfo;
        graphicsPipelineInfo.pInputAssemblyState = &inputAssemblyStateInfo;
        graphicsPipelineInfo.pViewportState = &viewportStateInfo;
        graphicsPipelineInfo.pRasterizationState = &rasterizationStateInfo;
        graphicsPipelineInfo.pMultisampleState = &multisampleStateInfo;
        graphicsPipelineInfo.pDepthStencilState = nullptr; // For now!
        graphicsPipelineInfo.pColorBlendState = &blendingStateInfo;
        graphicsPipelineInfo.pDynamicState = &dynamicStateInfo;
        graphicsPipelineInfo.layout = pipelineLayout->GetPipelineLayout();
        graphicsPipelineInfo.renderPass = renderPass->GetRenderPass();
        graphicsPipelineInfo.subpass = 0;
        graphicsPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        graphicsPipelineInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(
            device, VK_NULL_HANDLE, 1, &graphicsPipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
            throw std::runtime_error{ "Failed to create a Graphics Pipeline!" };
        }
    }
    void VulkanGraphicsPipeline::DestroyPipeline(VkDevice device) {
        vkDestroyPipeline(device, pipeline, nullptr);
    }

    void VulkanGraphicsPipeline::SetAttachmentCount(uint32_t attachmentCount) {
        blendingAttachmentStates.resize(attachmentCount);
    }

    void VulkanGraphicsPipeline::SetVertexShaderModule(const VulkanShaderModule& vertexShaderModule) {
        this->vertexShaderModule = vertexShaderModule;
    }
    void VulkanGraphicsPipeline::SetFragmentShaderModule(const VulkanShaderModule& fragmentShaderModule) {
        this->fragmentShaderModule = fragmentShaderModule;
    }

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

    void VulkanGraphicsPipeline::SetBlendingAttachmentState(
        const VkPipelineColorBlendAttachmentState& blendingAttachmentState, uint32_t attachmentIdx) {
        blendingAttachmentStates[attachmentIdx] = blendingAttachmentState;
    }
    void VulkanGraphicsPipeline::SetDefaultBlendingAttachmentState(
        bool blendingEnabled, uint32_t attachmentIdx) {
        if (blendingEnabled) EnableDefaultBlendingAttachmentState(attachmentIdx);
        else DisableDefaultBlendingAttachmentState(attachmentIdx);
    }

    void VulkanGraphicsPipeline::AddDynamicState(VkDynamicState dynamicState) {
        dynamicStates.push_back(dynamicState);
    }

    void VulkanGraphicsPipeline::SetRenderPass(std::shared_ptr<VulkanRenderPass> renderPass) {
        this->renderPass = renderPass;
    }
    void VulkanGraphicsPipeline::SetPipelineLayout(std::shared_ptr<VulkanPipelineLayout> pipelineLayout) {
        this->pipelineLayout = pipelineLayout;
    }

    void VulkanGraphicsPipeline::EnableDefaultBlendingAttachmentState(uint32_t attachmentIdx) {
        blendingAttachmentStates[attachmentIdx].colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blendingAttachmentStates[attachmentIdx].blendEnable = VK_TRUE;
        blendingAttachmentStates[attachmentIdx].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendingAttachmentStates[attachmentIdx].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendingAttachmentStates[attachmentIdx].colorBlendOp = VK_BLEND_OP_ADD;
        blendingAttachmentStates[attachmentIdx].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendingAttachmentStates[attachmentIdx].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendingAttachmentStates[attachmentIdx].alphaBlendOp = VK_BLEND_OP_ADD;
    }
    void VulkanGraphicsPipeline::DisableDefaultBlendingAttachmentState(uint32_t attachmentIdx) {
        blendingAttachmentStates[attachmentIdx].colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blendingAttachmentStates[attachmentIdx].blendEnable = VK_FALSE;
        blendingAttachmentStates[attachmentIdx].srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        blendingAttachmentStates[attachmentIdx].dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendingAttachmentStates[attachmentIdx].colorBlendOp = VK_BLEND_OP_ADD;
        blendingAttachmentStates[attachmentIdx].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendingAttachmentStates[attachmentIdx].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendingAttachmentStates[attachmentIdx].alphaBlendOp = VK_BLEND_OP_ADD;
    }

    VkPipelineShaderStageCreateInfo VulkanGraphicsPipeline::CreateVertexShaderStageCreateInfo() {
        VkPipelineShaderStageCreateInfo shaderStage{};
        shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStage.stage = GetVulkanShaderStage(vertexShaderModule.value().shaderType);
        shaderStage.module = vertexShaderModule.value().shaderModule;
        shaderStage.pName = vertexShaderModule.value().entryPoint.c_str();
        return shaderStage;
    }
    VkPipelineShaderStageCreateInfo VulkanGraphicsPipeline::CreateFragmentShaderStageCreateInfo() {
        VkPipelineShaderStageCreateInfo shaderStage{};
        shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStage.stage = GetVulkanShaderStage(fragmentShaderModule.value().shaderType);
        shaderStage.module = fragmentShaderModule.value().shaderModule;
        shaderStage.pName = fragmentShaderModule.value().entryPoint.c_str();
        return shaderStage;
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
    VkPipelineViewportStateCreateInfo VulkanGraphicsPipeline::CreateVulkanViewportStateInfo() const {
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
        colorBlending.attachmentCount = static_cast<uint32_t>(blendingAttachmentStates.size());
        colorBlending.pAttachments = blendingAttachmentStates.data();
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