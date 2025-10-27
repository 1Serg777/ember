#pragma once

#include "Framework/Asset/Vertex.h"

#include <vulkan/vulkan.h>

namespace ember {

	VkFormat PickVulkanVertexAttribFormat(const VertexAttribDescriptor& vertexAttrib);
	VkFormat PickVulkanVertexAttribFormat(VertexAttribFormat format, uint32_t dimension);

	VkFormat PickFloat32VulkanVertexAttribFormat(uint32_t dimension);

	VkFormat PickUint32VulkanVertexAttribFormat(uint32_t dimension);
	VkFormat PickUint16VulkanVertexAttribFormat(uint32_t dimension);
	VkFormat PickUint8VulkanVertexAttribFormat(uint32_t dimension);

	VkFormat PickInt32VulkanVertexAttribFormat(uint32_t dimension);
	VkFormat PickInt16VulkanVertexAttribFormat(uint32_t dimension);
	VkFormat PickInt8VulkanVertexAttribFormat(uint32_t dimension);

	VertexAttribFormat InferVertexAttribFormat(VkFormat vkFormat);
	uint32_t InferVertexAttribDimension(VkFormat vkFormat);

	std::vector<VkVertexInputAttributeDescription> VertexAttribLayoutToVulkanAttribDescription(
		const std::vector<VertexAttribDescriptor>& attribLayout);

	std::vector<VertexAttribDescriptor> VulkanAttribDescriptionToVertexAttribLayout(
		const std::vector<VkVertexInputAttributeDescription>& vulkanAttribLayout);
    
}