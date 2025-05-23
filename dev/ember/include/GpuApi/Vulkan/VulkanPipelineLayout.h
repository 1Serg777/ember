#pragma once

#include <vulkan/vulkan.h>

namespace ember {

	class VulkanPipelineLayout {
	public:
		void CreatePipelineLayout(VkDevice device);
		void DestroyPipelineLayout(VkDevice device);
		VkPipelineLayout GetPipelineLayout() const;

	private:
		VkPipelineLayout pipelineLayout{VK_NULL_HANDLE};
	};

}