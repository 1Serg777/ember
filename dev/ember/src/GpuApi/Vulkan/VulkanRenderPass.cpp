#include "GpuApi/Vulkan/VulkanRenderPass.h"

namespace ember {

	VkSubpassDescription& VulkanSubpasses::GetSubpassDescription(uint32_t subpassId) {
		return subpassDescs[subpassId];
	}
	VulkanSubpassAttachmentReferences& VulkanSubpasses::GetAttachmentReferences(uint32_t subpassId) {
		return attachmentRefs[subpassId];
	}

	void VulkanSubpasses::Resize(uint32_t subpassCount) {
		subpassDescs.resize(subpassCount);
		attachmentRefs.resize(subpassCount);
	}

	void VulkanRenderPass::SetAttachmentCount(uint32_t attachmentCount) {
		attachmentDescs.resize(attachmentCount);
	}
	void VulkanRenderPass::SetSubpassCount(uint32_t subpassCount) {
		subpasses.Resize(subpassCount);
	}
	void VulkanRenderPass::SetSubpassDependencyCount(uint32_t subpassDependencyCount) {
		subpassDeps.resize(subpassDependencyCount);
	}

	void VulkanRenderPass::SetAttachment(
		const VkAttachmentDescription& attachmentDescription, uint32_t attachmentIdx) {
		attachmentDescs[attachmentIdx] = attachmentDescription;
	}

	void VulkanRenderPass::SetShaderReadOnlyColorAttachment(
		VkFormat attachmentFormat, uint32_t attachmentIdx, VkSampleCountFlagBits samples) {
		VkAttachmentDescription colorAttachmentDesc{};
		colorAttachmentDesc.format = attachmentFormat;
		colorAttachmentDesc.samples = samples;
		colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		SetAttachment(colorAttachmentDesc, attachmentIdx);
	}
	void VulkanRenderPass::SetRenderTargetColorAttachment(
		VkFormat attachmentFormat, VkSampleCountFlagBits samples,
		bool presentationAttachment, uint32_t attachmentIdx) {
		VkAttachmentDescription colorAttachmentDesc{};
		colorAttachmentDesc.format = attachmentFormat;
		colorAttachmentDesc.samples = samples;
		colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		// colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		if (presentationAttachment)
			colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		else
			colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
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

	void VulkanRenderPass::SetSubpassColorAttachmentReferences(const std::vector<uint32_t>& refs, uint32_t subpassIdx) {
		VulkanSubpassAttachmentReferences& attachmentRefs = subpasses.GetAttachmentReferences(subpassIdx);
		std::vector<VkAttachmentReference>& colorAttachmentRefs = attachmentRefs.colorAttachments;
		colorAttachmentRefs.resize(refs.size());

		for (uint32_t i = 0; i < refs.size(); i++) {
			VkAttachmentReference attachmentRef{};
			attachmentRef.attachment = refs[i];
			attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colorAttachmentRefs[i] = attachmentRef;
		}

		VkSubpassDescription& subpassDesc = subpasses.GetSubpassDescription(subpassIdx);
		subpassDesc.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size());
		subpassDesc.pColorAttachments = colorAttachmentRefs.data();
	}

}