#include "GpuApi/Vulkan/VulkanFramebuffer.h"

#include <stdexcept>

namespace ember {

	void VulkanFramebuffer::SetAttachmentCount(uint32_t attachmentCount) {
		attachments.resize(attachmentCount);
	}

	void VulkanFramebuffer::CreateFramebuffer(VkDevice device) {
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass->GetRenderPass();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = width;
		framebufferInfo.height = height;
		framebufferInfo.layers = 1;
		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS) {
			throw std::runtime_error{ "Couldn't create a frame buffer!" };
		}
	}
	void VulkanFramebuffer::DestroyFramebuffer(VkDevice device) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
		framebuffer = VK_NULL_HANDLE;
	}

	void VulkanFramebuffer::SetFramebufferSize(uint32_t width, uint32_t height) {
		this->width = width;
		this->height = height;
	}

	void VulkanFramebuffer::SetAttachment(VkImageView attachment, uint32_t attachmentIdx) {
		attachments[attachmentIdx] = attachment;
	}
	void VulkanFramebuffer::ClearAttachments() {
		attachments.clear();
	}

	void VulkanFramebuffer::SetRenderPass(std::shared_ptr<VulkanRenderPass> vulkanRenderPass) {
		this->renderPass = vulkanRenderPass;
	}
	void VulkanFramebuffer::ResetRenderPass() {
		this->renderPass.reset();
	}

	VkFramebuffer VulkanFramebuffer::GetFramebuffer() const {
		return this->framebuffer;
	}

}