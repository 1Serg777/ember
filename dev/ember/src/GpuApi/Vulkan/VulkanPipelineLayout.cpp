#include "GpuApi/Vulkan/VulkanPipelineLayout.h"

#include <stdexcept>

namespace ember {

	void VulkanPipelineLayout::CreatePipelineLayout(VkDevice device) {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}
	void VulkanPipelineLayout::DestroyPipelineLayout(VkDevice device) {
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	}
	VkPipelineLayout VulkanPipelineLayout::GetPipelineLayout() const {
		return pipelineLayout;
	}

}