#pragma once

#include "Core/Util.h"
#include "GpuApi/GpuApiCtx.h"
#include "Window/Window.h"

#include <vulkan/vulkan.h>

namespace ember {

	class Window;
	class GpuApiImGuiCtx;

	struct SettingsVk {
		// TODO
	};

	struct VulkanInstanceData {
		VkInstance instance{VK_NULL_HANDLE};
#ifndef NDEBUG
		VkDebugUtilsMessengerEXT debugMessenger{VK_NULL_HANDLE};
#endif
		// Extensions and Layers
		std::vector<const char*> extensions;
		std::vector<const char*> layers;
	};

	struct VulkanData {
		VulkanInstanceData instanceData{};
	};

	class GpuApiCtxVk : public GpuApiCtx {
	public:
		GpuApiCtxVk(const SettingsVk& settings, Window* window);
		virtual ~GpuApiCtxVk() = default;

		CLASS_NO_COPY(GpuApiCtxVk);
		CLASS_NO_MOVE(GpuApiCtxVk);

		GpuApiType GetGpuApiType() const override;

		void Initialize() override;
		void InitializeGuiContext() override;
		void Terminate() override;
		void TerminateGuiContext() override;

		void OnMakeCurrent() override;
		void OnMakeNonCurrent() override;

		void OnFrameBegin() override;
		void OnFrameEnd() override;
		void DrawFrame() override;
		void Present() override;

		const SettingsVk& GetSettingsVk() const;

	private:
		void InitializeVulkanInstance(Window* window);

		std::vector<VkExtensionProperties> EnumerateSupportedVulkanInstanceExtensions() const;
		std::vector<const char*> EnumerateRequestedVulkanInstanceExtensions(WindowApiType windowApiType) const;

		void LogSupportedVulkanInstanceExtensions(const std::vector<VkExtensionProperties>& extensions) const;
		void LogRequestedVulkanInstanceExtensions(const std::vector<const char*>& extensions) const;

		bool RequestedVulkanInstanceExtensionsSupported(
			const std::vector<VkExtensionProperties>& supportedExtensions,
			const std::vector<const char*>& requestedExtensions) const;

#if defined(DEBUG) || defined(_DEBUG)
		std::vector<VkLayerProperties> EnumerateSupportedVulkanInstanceLayers() const;
		std::vector<const char*> EnumerateRequestedVulkanInstanceLayers() const;

		void LogSupportedVulkanInstanceLayers(const std::vector<VkLayerProperties>& layers) const;
		void LogRequestedVulkanInstanceLayers(const std::vector<const char*>& layers) const;

		bool RequestedVulkanInstanceLayersSupported(
			const std::vector<VkLayerProperties>& supportedLayers,
			const std::vector<const char*>& requestedLayers) const;
#endif

		void CreateVulkanInstance();

		VulkanData vulkanData;
		SettingsVk settings;
		Window* window{nullptr};
	};

	GpuApiCtxVk* CreateGpuApiCtxVk(Window* window);

	void SetCurrentGpuApiCtxVk(GpuApiCtxVk* gpuApiCtxVk);
	GpuApiCtxVk* GetCurrentGpuApiCtxVk();

}