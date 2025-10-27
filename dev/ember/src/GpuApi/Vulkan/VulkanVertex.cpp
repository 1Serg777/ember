#include "GpuApi/Vulkan/VulkanVertex.h"

#include <cassert>
#include <utility>

namespace ember {

	VkFormat PickVulkanVertexAttribFormat(const VertexAttribDescriptor& vertexAttrib) {
		return PickVulkanVertexAttribFormat(vertexAttrib.format, vertexAttrib.dimension);
	}
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

	VertexAttribFormat InferVertexAttribFormat(VkFormat vkFormat) {
		switch (vkFormat) {
			case VkFormat::VK_FORMAT_R32_SFLOAT:
			case VkFormat::VK_FORMAT_R32G32_SFLOAT:
			case VkFormat::VK_FORMAT_R32G32B32_SFLOAT:
			case VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT:
				return VertexAttribFormat::FLOAT32;
				break;
			
			case VkFormat::VK_FORMAT_R32_SINT:
			case VkFormat::VK_FORMAT_R32G32_SINT:
			case VkFormat::VK_FORMAT_R32G32B32_SINT:
			case VkFormat::VK_FORMAT_R32G32B32A32_SINT:
				return VertexAttribFormat::INT32;
				break;

			case VkFormat::VK_FORMAT_R32_UINT:
			case VkFormat::VK_FORMAT_R32G32_UINT:
			case VkFormat::VK_FORMAT_R32G32B32_UINT:
			case VkFormat::VK_FORMAT_R32G32B32A32_UINT:
				return VertexAttribFormat::UINT32;
				break;

			case VkFormat::VK_FORMAT_R16_SINT:
			case VkFormat::VK_FORMAT_R16G16_SINT:
			case VkFormat::VK_FORMAT_R16G16B16_SINT:
			case VkFormat::VK_FORMAT_R16G16B16A16_SINT:
				return VertexAttribFormat::INT16;
				break;

			case VkFormat::VK_FORMAT_R16_UINT:
			case VkFormat::VK_FORMAT_R16G16_UINT:
			case VkFormat::VK_FORMAT_R16G16B16_UINT:
			case VkFormat::VK_FORMAT_R16G16B16A16_UINT:
				return VertexAttribFormat::UINT16;
				break;

			case VkFormat::VK_FORMAT_R8_SINT:
			case VkFormat::VK_FORMAT_R8G8_SINT:
			case VkFormat::VK_FORMAT_R8G8B8_SINT:
			case VkFormat::VK_FORMAT_R8G8B8A8_SINT:
				return VertexAttribFormat::INT8;
			break;
			
			case VkFormat::VK_FORMAT_R8_UINT:
			case VkFormat::VK_FORMAT_R8G8_UINT:
			case VkFormat::VK_FORMAT_R8G8B8_UINT:
			case VkFormat::VK_FORMAT_R8G8B8A8_UINT:
				return VertexAttribFormat::UINT8;
				break;

			default:
				assert(true && "Vulkan format is not supported!");
				return VertexAttribFormat{};
				break;
		}
	}
	uint32_t InferVertexAttribDimension(VkFormat vkFormat) {
		switch (vkFormat) {
			case VkFormat::VK_FORMAT_R32_SFLOAT:
			case VkFormat::VK_FORMAT_R32_UINT:
			case VkFormat::VK_FORMAT_R16_UINT:
			case VkFormat::VK_FORMAT_R8_UINT:
			case VkFormat::VK_FORMAT_R32_SINT:
			case VkFormat::VK_FORMAT_R16_SINT:
			case VkFormat::VK_FORMAT_R8_SINT:
				return 1;
				break;
			case VkFormat::VK_FORMAT_R32G32_SFLOAT:
			case VkFormat::VK_FORMAT_R32G32_UINT:
			case VkFormat::VK_FORMAT_R16G16_UINT:
			case VkFormat::VK_FORMAT_R8G8_UINT:
			case VkFormat::VK_FORMAT_R32G32_SINT:
			case VkFormat::VK_FORMAT_R16G16_SINT:
			case VkFormat::VK_FORMAT_R8G8_SINT:
				return 2;
				break;
			case VkFormat::VK_FORMAT_R32G32B32_SFLOAT:
			case VkFormat::VK_FORMAT_R32G32B32_UINT:
			case VkFormat::VK_FORMAT_R16G16B16_UINT:
			case VkFormat::VK_FORMAT_R8G8B8_UINT:
			case VkFormat::VK_FORMAT_R32G32B32_SINT:
			case VkFormat::VK_FORMAT_R16G16B16_SINT:
			case VkFormat::VK_FORMAT_R8G8B8_SINT:
				return 3;
				break;
			case VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT:
			case VkFormat::VK_FORMAT_R32G32B32A32_UINT:
			case VkFormat::VK_FORMAT_R16G16B16A16_UINT:
			case VkFormat::VK_FORMAT_R8G8B8A8_UINT:
			case VkFormat::VK_FORMAT_R32G32B32A32_SINT:
			case VkFormat::VK_FORMAT_R16G16B16A16_SINT:
			case VkFormat::VK_FORMAT_R8G8B8A8_SINT:
				return 4;
				break;
			default:
				assert(true && "Vulkan format is not supported!");
				return 0;
				break;
		}
	}

	std::vector<VkVertexInputAttributeDescription> VertexAttribLayoutToVulkanAttribDescription(
		const std::vector<VertexAttribDescriptor>& attribLayout) {
		std::vector<VkVertexInputAttributeDescription> vulkanVertexLayout(attribLayout.size());
		for (uint32_t i = 0; i < attribLayout.size(); i++) {
			vulkanVertexLayout[i].binding = 0;
			vulkanVertexLayout[i].location = static_cast<uint32_t>(attribLayout[i].channel);
			vulkanVertexLayout[i].format = PickVulkanVertexAttribFormat(attribLayout[i].format, attribLayout[i].dimension);
			vulkanVertexLayout[i].offset = attribLayout[i].offset;
		}
		return vulkanVertexLayout;
	}

	std::vector<VertexAttribDescriptor> VulkanAttribDescriptionToVertexAttribLayout(
		const std::vector<VkVertexInputAttributeDescription>& vulkanAttribLayout) {
		std::vector<VertexAttribDescriptor> vertexLayout(vulkanAttribLayout.size());
		for (uint32_t i = 0; i < vulkanAttribLayout.size(); i++) {
			vertexLayout[i].dimension = InferVertexAttribDimension(vulkanAttribLayout[i].format);
			vertexLayout[i].format = InferVertexAttribFormat(vulkanAttribLayout[i].format);
			vertexLayout[i].channel = static_cast<VertexAttribChannel>(vulkanAttribLayout[i].location);
			vertexLayout[i].offset = vulkanAttribLayout[i].offset;
		}
		return vertexLayout;
	}

}