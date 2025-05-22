#pragma once

#include "Renderer/Vertex.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace ember {

    class VulkanGraphicsPipeline {
    public:
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

        void AddDynamicState(VkDynamicState dynamicState);

    private:
        VkPipelineVertexInputStateCreateInfo CreateVulkanVertexInputStateInfo() const;
        VkPipelineInputAssemblyStateCreateInfo CreateVulkanInputAssemblyStateInfo() const;
        VkPipelineViewportStateCreateInfo CreateVulkanViewportAndScissorsStateInfo() const;
        VkPipelineRasterizationStateCreateInfo CreateVulkanRasterizationStateInfo() const;
        VkPipelineMultisampleStateCreateInfo CreateVulkanMultisampleStateInfo() const;
        VkPipelineColorBlendStateCreateInfo CreateVulkanBlendingStateInfo() const;
        VkPipelineDynamicStateCreateInfo CreateVulkanDynamicStateInfo() const;

        std::vector<VkVertexInputBindingDescription> vertexBindingDescs;
        std::vector<VkVertexInputAttributeDescription> vertexAttribDescs;

        std::vector<VkDynamicState> dynamicStates;

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