#include "GpuApi/Vulkan/VulkanShader.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace ember {

    void VulkanShaderFactory::CreateShaderModule(VulkanShaderModule& shader, VkDevice device) {
        std::cout << "Loading shader from: " << shader.shaderPath.c_str() << std::endl;
        std::vector<char> shaderBuf = ReadShaderFile(shader.shaderPath);
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderBuf.size();
        createInfo.pCode = reinterpret_cast<uint32_t*>(shaderBuf.data());
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shader.shaderModule) != VK_SUCCESS) {
            std::string errMsg{ "Couldn't create shader module: " + shader.shaderPath.string() };
            throw std::runtime_error{ errMsg };
        }
    }
    void VulkanShaderFactory::DestroyShaderModule(VulkanShaderModule& shader, VkDevice device) {
        vkDestroyShaderModule(device, shader.shaderModule, nullptr);
        shader.shaderModule = VK_NULL_HANDLE;
    }

    std::vector<char> VulkanShaderFactory::ReadShaderFile(const std::filesystem::path& shaderFilePath) {
        std::ifstream shaderFile(shaderFilePath, std::ifstream::ate | std::ifstream::binary);
        if (!shaderFile.is_open()) {
            std::string errMsg{ "Couldn't open the shader file: " + shaderFilePath.string() };
            throw std::runtime_error{ errMsg };
        }
        size_t fileSize = shaderFile.tellg();
        std::vector<char> shaderBuf(fileSize);
        shaderFile.seekg(0);
        shaderFile.read(shaderBuf.data(), fileSize);
        shaderFile.close();
        return shaderBuf;
    }

    VkShaderStageFlagBits GetVulkanShaderStage(SHADER_TYPE shaderType) {
        switch (shaderType) {
            case SHADER_TYPE::VERTEX_SHADER:
                return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
            break;
            case SHADER_TYPE::FRAGMENT_SHADER:
                return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
            default:
                return static_cast<VkShaderStageFlagBits>(0);
            break;
        }
    }

}