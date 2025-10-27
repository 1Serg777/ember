#pragma once

#include "Framework/Asset/Vertex.h"

#include "GpuApi/Vulkan/VulkanRenderPass.h"
#include "GpuApi/Vulkan/VulkanPipelineLayout.h"
#include "GpuApi/Vulkan/VulkanShader.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>
#include <optional>

namespace ember {

    class VulkanGraphicsPipeline {
    public:
        void SetAttachmentCount(uint32_t attachmentCount);

        void SetVertexShaderModule(const VulkanShaderModule& vertexShaderModule);
        void SetFragmentShaderModule(const VulkanShaderModule& fragmentShaderModule);

        void SetVertexLayoutInterleaved(const VertexBufferInfo& vbInfo);
        void SetPrimitiveTopology(VkPrimitiveTopology topology);

        void SetViewport(VkViewport viewport);
        void SetScissors(VkRect2D scissors);

        void SetPolygonMode(VkPolygonMode vulkanPolygonMode);
        void SetCullMode(VkCullModeFlagBits vulkanCullMode);
        void SetFrontFace(VkFrontFace vulkanFrontFace);
        void SetLineWidth(float lineWidth);

        void EnableMultisampling(VkSampleCountFlagBits vulkanSampleCount);
        void DisableMultisampling();

        void SetBlendingAttachmentState(
            const VkPipelineColorBlendAttachmentState& blendingAttachmentState, uint32_t attachmentIdx);
        void SetDefaultBlendingAttachmentState(
            bool blendingEnabled, uint32_t attachmentIdx);

        void AddDynamicState(VkDynamicState dynamicState);

        void SetRenderPass(std::shared_ptr<VulkanRenderPass> renderPass);
        void SetPipelineLayout(std::shared_ptr<VulkanPipelineLayout> pipelineLayout);

        void CreatePipeline(VkDevice device);
        void DestroyPipeline(VkDevice device);
        VkPipeline GetPipeline() const;

    private:
        void EnableDefaultBlendingAttachmentState(uint32_t attachmentIdx);
        void DisableDefaultBlendingAttachmentState(uint32_t attachmentIdx);

        VkPipelineShaderStageCreateInfo CreateVertexShaderStageCreateInfo();
        VkPipelineShaderStageCreateInfo CreateFragmentShaderStageCreateInfo();

        VkPipelineVertexInputStateCreateInfo CreateVulkanVertexInputStateInfo() const;
        VkPipelineInputAssemblyStateCreateInfo CreateVulkanInputAssemblyStateInfo() const;
        VkPipelineViewportStateCreateInfo CreateVulkanViewportStateInfo() const;
        VkPipelineRasterizationStateCreateInfo CreateVulkanRasterizationStateInfo() const;
        VkPipelineMultisampleStateCreateInfo CreateVulkanMultisampleStateInfo() const;
        VkPipelineColorBlendStateCreateInfo CreateVulkanBlendingStateInfo() const;
        VkPipelineDynamicStateCreateInfo CreateVulkanDynamicStateInfo() const;

        std::vector<VkVertexInputBindingDescription> vertexBindingDescs;
        std::vector<VkVertexInputAttributeDescription> vertexAttribDescs;
        std::vector<VkPipelineColorBlendAttachmentState> blendingAttachmentStates;
        std::vector<VkDynamicState> dynamicStates;

        std::optional<VulkanShaderModule> vertexShaderModule;
        std::optional<VulkanShaderModule> fragmentShaderModule;

        std::shared_ptr<VulkanRenderPass> renderPass;
        std::shared_ptr<VulkanPipelineLayout> pipelineLayout;

        VkPipeline pipeline{VK_NULL_HANDLE};

        VkViewport viewport{};
        VkRect2D scissors{};

        VkPrimitiveTopology topology{};

        VkPolygonMode polygonMode{};
        VkCullModeFlagBits cullMode{};
        VkFrontFace frontFace{};
        float lineWidth{};

        VkSampleCountFlagBits samples{};
        bool multisamplingEnabled{};
    };
    
}