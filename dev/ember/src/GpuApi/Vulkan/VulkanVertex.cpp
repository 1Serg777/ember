#include "GpuApi/Vulkan/VulkanVertex.h"

namespace ember {

	VkFormat PickVulkanVertexAttribFormat(VertexAttribFormat format, uint32_t dimension) {
		switch (format) {
			case VertexAttribFormat::FLOAT32:
				return PickFloat32VulkanVertexAttribFormat(dimension);
			break;

			case VertexAttribFormat::UINT32:
				return PickUint32VulkanVertexAttribFormat(dimension);
			break;
			case VertexAttribFormat::UINT16:
				return PickUint16VulkanVertexAttribFormat(dimension);
			break;
			case VertexAttribFormat::UINT8:
				return PickUint8VulkanVertexAttribFormat(dimension);
			break;

			case VertexAttribFormat::INT32:
				return PickInt32VulkanVertexAttribFormat(dimension);
			break;
			case VertexAttribFormat::INT16:
				return PickInt16VulkanVertexAttribFormat(dimension);
			break;
			case VertexAttribFormat::INT8:
				return PickInt8VulkanVertexAttribFormat(dimension);
			break;

			default:
				assert(true && "Unknown vertex attribute format provided!");
				return VkFormat{};
			break;
		}
	}

	VkFormat PickFloat32VulkanVertexAttribFormat(uint32_t dimension) {
		switch (dimension) {
			case 1:
				return VkFormat::VK_FORMAT_R32_SFLOAT;
			break;
			case 2:
				return VkFormat::VK_FORMAT_R32G32_SFLOAT;
			break;
			case 3:
				return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
			break;
			case 4:
				return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
			default:
				assert(true && "Vertex attribute dimension is not supported!");
				return VkFormat{};
			break;
		}
	}

	VkFormat PickUint32VulkanVertexAttribFormat(uint32_t dimension) {
		switch (dimension) {
			case 1:
				return VkFormat::VK_FORMAT_R32_UINT;
			break;
			case 2:
				return VkFormat::VK_FORMAT_R32G32_UINT;
			break;
			case 3:
				return VkFormat::VK_FORMAT_R32G32B32_UINT;
			break;
			case 4:
				return VkFormat::VK_FORMAT_R32G32B32A32_UINT;
			break;
			default:
				assert(true && "Vertex attribute dimension is not supported!");
				return VkFormat{};
			break;
		}
	}
	VkFormat PickUint16VulkanVertexAttribFormat(uint32_t dimension) {
		switch (dimension) {
			case 1:
				return VkFormat::VK_FORMAT_R16_UINT;
			break;
			case 2:
				return VkFormat::VK_FORMAT_R16G16_UINT;
			break;
			case 3:
				return VkFormat::VK_FORMAT_R16G16B16_UINT;
			break;
			case 4:
				return VkFormat::VK_FORMAT_R16G16B16A16_UINT;
			break;
			default:
				assert(true && "Vertex attribute dimension is not supported!");
				return VkFormat{};
			break;
		}
	}
	VkFormat PickUint8VulkanVertexAttribFormat(uint32_t dimension) {
		switch (dimension) {
			case 1:
				return VkFormat::VK_FORMAT_R8_UINT;
			break;
			case 2:
				return VkFormat::VK_FORMAT_R8G8_UINT;
			break;
			case 3:
				return VkFormat::VK_FORMAT_R8G8B8_UINT;
			break;
			case 4:
				return VkFormat::VK_FORMAT_R8G8B8A8_UINT;
			break;
			default:
				assert(true && "Vertex attribute dimension is not supported!");
				return VkFormat{};
			break;
		}
	}

	VkFormat PickInt32VulkanVertexAttribFormat(uint32_t dimension) {
		switch (dimension) {
			case 1:
				return VkFormat::VK_FORMAT_R32_SINT;
			break;
			case 2:
				return VkFormat::VK_FORMAT_R32G32_SINT;
			break;
			case 3:
				return VkFormat::VK_FORMAT_R32G32B32_SINT;
			break;
			case 4:
				return VkFormat::VK_FORMAT_R32G32B32A32_SINT;
			break;
			default:
				assert(true && "Vertex attribute dimension is not supported!");
				return VkFormat{};
			break;
		}
	}
	VkFormat PickInt16VulkanVertexAttribFormat(uint32_t dimension) {
		switch (dimension) {
			case 1:
				return VkFormat::VK_FORMAT_R16_SINT;
			break;
			case 2:
				return VkFormat::VK_FORMAT_R16G16_SINT;
			break;
			case 3:
				return VkFormat::VK_FORMAT_R16G16B16_SINT;
			break;
			case 4:
				return VkFormat::VK_FORMAT_R16G16B16A16_SINT;
			break;
			default:
				assert(true && "Vertex attribute dimension is not supported!");
				return VkFormat{};
			break;
		}
	}
	VkFormat PickInt8VulkanVertexAttribFormat(uint32_t dimension) {
		switch (dimension) {
			case 1:
				return VkFormat::VK_FORMAT_R8_SINT;
			break;
			case 2:
				return VkFormat::VK_FORMAT_R8G8_SINT;
			break;
			case 3:
				return VkFormat::VK_FORMAT_R8G8B8_SINT;
			break;
			case 4:
				return VkFormat::VK_FORMAT_R8G8B8A8_SINT;
			break;
			default:
				assert(true && "Vertex attribute dimension is not supported!");
				return VkFormat{};
			break;
		}
	}

	std::vector<VkVertexInputAttributeDescription> VertexAttribLayoutToVulkanAttribDescription(
		const std::vector<VertexAttribDescriptor>& attribLayout) {
		std::vector<VkVertexInputAttributeDescription> vulkanVertexLayout(attribLayout.size());
		for (uint32_t i = 0; i < attribLayout.size(); i++) {
			vulkanVertexLayout[i].binding = 0;
			vulkanVertexLayout[i].location = i;
			vulkanVertexLayout[i].format = PickVulkanVertexAttribFormat(attribLayout[i].format, attribLayout[i].dimension);
			vulkanVertexLayout[i].offset = attribLayout[i].offset;
		}
		return vulkanVertexLayout;
	}

	std::vector<VertexAttribDescriptor> VulkanAttribDescriptionToVertexAttribLayout(
		const std::vector<VkVertexInputAttributeDescription>& vulkanAttribLayout) {
		std::vector<VertexAttribDescriptor> vertexLayout;
		// TODO
		return vertexLayout;
	}

}