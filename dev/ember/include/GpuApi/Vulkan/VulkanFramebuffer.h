#pragma once

#include "GpuApi/Vulkan/VulkanRenderPass.h"

#include "vulkan/vulkan.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace ember {

	class VulkanFramebuffer {
	public:
		void SetAttachmentCount(uint32_t attachmentCount);

		void CreateFramebuffer(VkDevice device);
		void DestroyFramebuffer(VkDevice device);

		void SetFramebufferSize(uint32_t width, uint32_t height);

		void SetAttachment(VkImageView attachment, uint32_t attachmentIdx);
		void ClearAttachments();

		void SetRenderPass(std::shared_ptr<VulkanRenderPass> vulkanRenderPass);
		void ResetRenderPass();

		VkFramebuffer GetFramebuffer() const;

	private:
		std::vector<VkImageView> attachments;
		std::shared_ptr<VulkanRenderPass> renderPass;
		VkFramebuffer framebuffer{VK_NULL_HANDLE};
		uint32_t width{};
		uint32_t height{};
	};

}