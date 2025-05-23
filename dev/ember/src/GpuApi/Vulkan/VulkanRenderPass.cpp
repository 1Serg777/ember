#include "GpuApi/Vulkan/VulkanRenderPass.h"

#include <stdexcept>

namespace ember {

	void VulkanRenderPass::SetAttachmentCount(uint32_t attachmentCount) {
		attachmentDescs.resize(attachmentCount);
	}
	void VulkanRenderPass::SetSubpassCount(uint32_t subpassCount) {
		subpassDescs.resize(subpassCount);
		attachmentRefs.resize(subpassCount);
	}
	void VulkanRenderPass::SetSubpassColorAttachmentCount(uint32_t subpass, uint32_t attachmentCount) {
		std::vector<VkAttachmentReference>& colorAttachmentRefs = GetColorAttachmentReferences(subpass);
		colorAttachmentRefs.resize(attachmentCount);
		VkSubpassDescription& subpassDesc = GetSubpassDescription(subpass);
		subpassDesc.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size());
		subpassDesc.pColorAttachments = colorAttachmentRefs.data();
	}
	void VulkanRenderPass::SetSubpassDependencyCount(uint32_t subpassDependencyCount) {
		subpassDeps.resize(subpassDependencyCount);
	}

	void VulkanRenderPass::SetAttachment(
		const VkAttachmentDescription& attachmentDescription, uint32_t attachmentIdx) {
		attachmentDescs[attachmentIdx] = attachmentDescription;
	}

	void VulkanRenderPass::SetRenderTargetColorAttachment(VkFormat attachmentFormat, uint32_t attachmentIdx) {
		VkAttachmentDescription colorAttachmentDesc{};
		colorAttachmentDesc.format = attachmentFormat;
		colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		SetAttachment(colorAttachmentDesc, attachmentIdx);
	}
	void VulkanRenderPass::SetPresentRenderTargetColorAttachment(VkFormat attachmentFormat, uint32_t attachmentIdx) {
		VkAttachmentDescription colorAttachmentDesc{};
		colorAttachmentDesc.format = attachmentFormat;
		colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		SetAttachment(colorAttachmentDesc, attachmentIdx);
	}
	void VulkanRenderPass::SetDepthAttachment(uint32_t attachmentIdx, bool storeDepth) {
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = VK_FORMAT_D32_SFLOAT;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = storeDepth ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
	}
	void VulkanRenderPass::SetDepthStencilAttachment(uint32_t attachmentIdx, bool storeDepth, bool storeStencil) {
		VkAttachmentDescription depthStencilAttachment{};
		depthStencilAttachment.format = VK_FORMAT_D24_UNORM_S8_UINT;
		depthStencilAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthStencilAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthStencilAttachment.storeOp = storeDepth ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthStencilAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthStencilAttachment.stencilStoreOp = storeStencil ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthStencilAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthStencilAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}

	void VulkanRenderPass::SetSubpassColorAttachmentReference(uint32_t attachmentId, uint32_t refId, uint32_t subpass) {
		SetSubpassColorAttachmentReference(attachmentId, refId, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, subpass);
	}
	void VulkanRenderPass::SetSubpassColorAttachmentReference(
		uint32_t attachmentId, uint32_t refId, VkImageLayout layout, uint32_t subpass) {
		// This 'refId' is important as it corresponds to the layout location attribute
		// of the corresponding variable declaration in the shader.
		std::vector<VkAttachmentReference>& colorAttachmentRefs = GetColorAttachmentReferences(subpass);
		VkAttachmentReference attachmentRef{};
		attachmentRef.attachment = attachmentId;
		attachmentRef.layout = layout;
		colorAttachmentRefs[refId] = attachmentRef;
	}
	void VulkanRenderPass::SetSubpassColorAttachmentReferences(
		const std::vector<uint32_t>& attachments, uint32_t subpass) {
		SetSubpassColorAttachmentReferences(attachments, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, subpass);
	}
	void VulkanRenderPass::SetSubpassColorAttachmentReferences(
		const std::vector<uint32_t>& attachments, VkImageLayout layout, uint32_t subpass) {
		SetSubpassColorAttachmentCount(subpass, static_cast<uint32_t>(attachments.size()));
		for (uint32_t refId = 0; refId < attachments.size(); refId++) {
			SetSubpassColorAttachmentReference(attachments[refId], refId, layout, subpass);
		}
	}

	void VulkanRenderPass::CreateRenderPass(VkDevice device) {
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescs.size());
		renderPassInfo.pAttachments = attachmentDescs.data();
		renderPassInfo.subpassCount = static_cast<uint32_t>(subpassDescs.size());
		renderPassInfo.pSubpasses = subpassDescs.data();

		/*
		VkSubpassDependency subpassDependency{};
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.dstSubpass = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.srcAccessMask = 0;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		*/

		// renderPassInfo.dependencyCount = 1;
		// renderPassInfo.pDependencies = &subpassDependency;

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
			throw std::runtime_error{ "Failed to create a Render Pass!" };
		}
	}
	void VulkanRenderPass::DestroyRenderPass(VkDevice device) {
		vkDestroyRenderPass(device, renderPass, nullptr);
	}
	VkRenderPass VulkanRenderPass::GetRenderPass() const {
		return renderPass;
	}

	VkSubpassDescription& VulkanRenderPass::GetSubpassDescription(uint32_t subpass) {
		return subpassDescs[subpass];
	}
	VulkanAttachmentReferences& VulkanRenderPass::GetAttachmentReferences(uint32_t subpass) {
		return attachmentRefs[subpass];
	}
	std::vector<VkAttachmentReference>& VulkanRenderPass::GetColorAttachmentReferences(uint32_t subpass) {
		return GetAttachmentReferences(subpass).colorAttachments;
	}

}