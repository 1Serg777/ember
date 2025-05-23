#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace ember {

	struct VulkanAttachmentReferences {
		std::vector<VkAttachmentReference> colorAttachments;
	};

	class VulkanRenderPass {
	public:
		void SetAttachmentCount(uint32_t attachmentCount);
		void SetSubpassCount(uint32_t subpassCount);
		void SetSubpassColorAttachmentCount(uint32_t subpass, uint32_t attachmentCount);
		void SetSubpassDependencyCount(uint32_t subpassDependencyCount);

		void SetAttachment(
			const VkAttachmentDescription& attachmentDescription,
			uint32_t attachmentIdx);

		void SetRenderTargetColorAttachment(VkFormat attachmentFormat, uint32_t attachmentIdx);
		void SetPresentRenderTargetColorAttachment(VkFormat attachmentFormat, uint32_t attachmentIdx);
		void SetDepthAttachment(uint32_t attachmentIdx, bool storeDepth);
		void SetDepthStencilAttachment(uint32_t attachmentIdx, bool storeDepth, bool storeStencil);

		void SetSubpassColorAttachmentReference(uint32_t attachmentId, uint32_t refId, uint32_t subpassIdx);
		void SetSubpassColorAttachmentReference(
			uint32_t attachmentRef, uint32_t refId, VkImageLayout layout, uint32_t subpassIdx);
		void SetSubpassColorAttachmentReferences(const std::vector<uint32_t>& attachments, uint32_t subpassIdx);
		void SetSubpassColorAttachmentReferences(
			const std::vector<uint32_t>& attachments, VkImageLayout layout, uint32_t subpassIdx);

		void CreateRenderPass(VkDevice device);
		void DestroyRenderPass(VkDevice device);
		VkRenderPass GetRenderPass() const;

	private:
		VkSubpassDescription& GetSubpassDescription(uint32_t subpass);
		VulkanAttachmentReferences& GetAttachmentReferences(uint32_t subpass);
		std::vector<VkAttachmentReference>& GetColorAttachmentReferences(uint32_t subpass);

		std::vector<VkAttachmentDescription> attachmentDescs;
		std::vector<VkSubpassDescription> subpassDescs;
		std::vector<VulkanAttachmentReferences> attachmentRefs;
		std::vector<VkSubpassDependency> subpassDeps;
		VkRenderPass renderPass{VK_NULL_HANDLE};
	};

}