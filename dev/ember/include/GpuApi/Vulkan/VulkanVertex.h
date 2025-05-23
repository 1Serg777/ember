#pragma once

#include "GpuApi/Vertex.h"

#include <vulkan/vulkan.h>

namespace ember {

	VkFormat PickVulkanVertexAttribFormat(VertexAttribFormat format, uint32_t dimension);

	VkFormat PickFloat32VulkanVertexAttribFormat(uint32_t dimension);

	VkFormat PickUint32VulkanVertexAttribFormat(uint32_t dimension);
	VkFormat PickUint16VulkanVertexAttribFormat(uint32_t dimension);
	VkFormat PickUint8VulkanVertexAttribFormat(uint32_t dimension);

	VkFormat PickInt32VulkanVertexAttribFormat(uint32_t dimension);
	VkFormat PickInt16VulkanVertexAttribFormat(uint32_t dimension);
	VkFormat PickInt8VulkanVertexAttribFormat(uint32_t dimension);

	std::vector<VkVertexInputAttributeDescription> VertexAttribLayoutToVulkanAttribDescription(
		const std::vector<VertexAttribDescriptor>& attribLayout);

	std::vector<VertexAttribDescriptor> VulkanAttribDescriptionToVertexAttribLayout(
		const std::vector<VkVertexInputAttributeDescription>& vulkanAttribLayout);
    
}