#pragma once

#include "Framework/Asset/Shader.h"

#include <vulkan/vulkan.h>

#include <filesystem>
#include <string>
#include <vector>

namespace ember {

	struct VulkanShaderModule {
		std::filesystem::path shaderPath;
		std::string entryPoint;
		VkShaderModule shaderModule{};
		SHADER_TYPE shaderType{};
	};

	class VulkanShaderFactory {
	public:
		static void CreateShaderModule(VulkanShaderModule& shader, VkDevice device);
		static void DestroyShaderModule(VulkanShaderModule& shader, VkDevice device);

	private:
		static std::vector<char> ReadShaderFile(const std::filesystem::path& shaderFilePath);
	};

	VkShaderStageFlagBits GetVulkanShaderStage(SHADER_TYPE shaderType);

}