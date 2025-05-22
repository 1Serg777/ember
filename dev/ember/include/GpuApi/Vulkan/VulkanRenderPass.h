#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace ember {

	struct VulkanSubpassAttachmentReferences {
		std::vector<VkAttachmentReference> colorAttachments;
	};

	struct VulkanSubpasses {
		VkSubpassDescription& GetSubpassDescription(uint32_t subpassId);
		VulkanSubpassAttachmentReferences& GetAttachmentReferences(uint32_t subpassId);

		void Resize(uint32_t subpassCount);

		std::vector<VkSubpassDescription> subpassDescs;
		std::vector<VulkanSubpassAttachmentReferences> attachmentRefs;
	};

	class VulkanRenderPass {
	public:
		void SetAttachmentCount(uint32_t attachmentCount);
		void SetSubpassCount(uint32_t subpassCount);
		void SetSubpassDependencyCount(uint32_t subpassDependencyCount);

		void SetAttachment(
			const VkAttachmentDescription& attachmentDescription,
			uint32_t attachmentIdx);

		void SetShaderReadOnlyColorAttachment(
			VkFormat attachmentFormat, uint32_t attachmentIdx, VkSampleCountFlagBits samples);
		void SetRenderTargetColorAttachment(
			VkFormat attachmentFormat, VkSampleCountFlagBits samples,
			bool presentationAttachment, uint32_t attachmentIdx);

		void SetDepthAttachment(uint32_t attachmentIdx, bool storeDepth);
		void SetDepthStencilAttachment(uint32_t attachmentIdx, bool storeDepth, bool storeStencil);

		void SetSubpassColorAttachmentReferences(const std::vector<uint32_t>& refs, uint32_t subpassIdx);

	private:
		VulkanSubpasses subpasses;
		std::vector<VkAttachmentDescription> attachmentDescs;
		std::vector<VkSubpassDependency> subpassDeps;
		VkRenderPass renderPass{VK_NULL_HANDLE};
	};

}