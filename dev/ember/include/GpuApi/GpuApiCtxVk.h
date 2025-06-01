#pragma once

#include "Core/Util.h"
#include "GpuApi/GpuApiCtx.h"
#include "Window/Window.h"

#include "GpuApi/Vulkan/VulkanPipeline.h"
#include "GpuApi/Vulkan/VulkanRenderPass.h"
#include "GpuApi/Vulkan/VulkanPipelineLayout.h"
#include "GpuApi/Vulkan/VulkanFramebuffer.h"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <memory>
#include <optional>

namespace ember {

	class Window;
	class GpuApiImGuiCtx;

	struct SettingsVk {
		// TODO
	};

	struct VulkanQueueFamilyIndices {
		bool HasGraphicsQueueFamily() const;
		bool HasPresentQueueFamily() const;
		bool Complete() const;

		std::optional<uint32_t> graphicsQueueFamily;
		std::optional<uint32_t> presentQueueFamily;
	};

	struct VulkanQueueFamily {
		VkQueue queueHandle{VK_NULL_HANDLE};
		VkCommandPool commandPool{VK_NULL_HANDLE};
		uint32_t queueFamilyId{0};
	};

	struct VulkanSwapchainQueryInfo {
		VkSurfaceCapabilitiesKHR capabilities{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct VulkanPhysicalDeviceInfo {
		VkPhysicalDeviceProperties deviceProperties{};
		VkPhysicalDeviceFeatures deviceFeatures{};
		std::optional<VulkanSwapchainQueryInfo> swapchainInfo;
		std::vector<VkExtensionProperties> deviceExtensions;
		VulkanQueueFamilyIndices queueFamilyIds;
		VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
	};

	struct VulkanInstanceData {
		VkInstance instance{VK_NULL_HANDLE};
#if defined(DEBUG) || defined(_DEBUG)
		VkDebugUtilsMessengerEXT debugMessenger{VK_NULL_HANDLE};
#endif
		std::vector<const char*> extensions;
		std::vector<const char*> layers;
	};

	struct VulkanSwapchainData {
		VkSwapchainKHR swapchain{VK_NULL_HANDLE};

		VkExtent2D swapchainExtent{};
		VkSurfaceFormatKHR swapchainSurfaceFormat{};
		VkPresentModeKHR swapchainPresentMode{};

		uint32_t requestedSwapchainImageCount{};
		uint32_t swapchainImageCount{};
	};

	struct VulkanDeviceData {
		VulkanPhysicalDeviceInfo physicalDeviceInfo{};
		VkPhysicalDeviceFeatures requestedFeatures{};

		VulkanSwapchainData swapchainData{};

		VulkanQueueFamily graphicsQueueFamily{};
		VulkanQueueFamily presentationQueueFamily{};

		std::vector<const char*> requestedDeviceExtensions;
		std::vector<const char*> requestedDeviceLayers;

		VkDevice logicalDevice{VK_NULL_HANDLE};
	};

	struct VulkanFrameResources {
		VkFence frameFinishedFence{VK_NULL_HANDLE};
		VkSemaphore imageAvailableSemaphore{VK_NULL_HANDLE};
		VkCommandBuffer commandBuffer{VK_NULL_HANDLE};
	};
	struct VulkanSwapchainImageResources {
		VulkanFramebuffer framebuffer;
		VkImage image{VK_NULL_HANDLE};
		VkImageView imageView{VK_NULL_HANDLE};
		VkSemaphore renderingFinishedSemaphore{VK_NULL_HANDLE};
	};

	struct VulkanData {
		VkInstance GetInstance() const;
		VkPhysicalDevice GetPhysicalDevice() const;
		VkDevice GetLogicalDevice() const;
		VkSwapchainKHR GetSwapchain() const;

		VulkanDeviceData& GetDeviceData();
		const VulkanDeviceData& GetDeviceData() const;

		VulkanSwapchainData& GetSwapchainData();
		const VulkanSwapchainData& GetSwapchainData() const;

		VulkanPhysicalDeviceInfo& GetPhysicalDeviceInfo();
		const VulkanPhysicalDeviceInfo& GetPhysicalDeviceInfo() const;

		VulkanQueueFamily& GetGraphicsQueueFamily();
		const VulkanQueueFamily& GetGraphicsQueueFamily() const;

		VulkanQueueFamily& GetPresentationQueueFamily();
		const VulkanQueueFamily& GetPresentationQueueFamily() const;

		VulkanDeviceData deviceData{};
		VulkanInstanceData instanceData{};
		VkSurfaceKHR surface{VK_NULL_HANDLE};
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

		void OnFramebufferResize() override;

		const SettingsVk& GetSettingsVk() const;

	private:
		void EnumerateVulkanInstanceExtensions();

		std::vector<VkExtensionProperties> EnumerateSupportedVulkanInstanceExtensions() const;
		std::vector<const char*> EnumerateRequestedVulkanInstanceExtensions(WindowApiType windowApiType) const;

		void LogSupportedVulkanInstanceExtensions(const std::vector<VkExtensionProperties>& extensions) const;
		void LogRequestedVulkanInstanceExtensions(const std::vector<const char*>& extensions) const;

		bool RequestedVulkanInstanceExtensionsSupported(
			const std::vector<VkExtensionProperties>& supportedExtensions,
			const std::vector<const char*>& requestedExtensions) const;

#if defined(DEBUG) || defined(_DEBUG)
		void EnumerateVulkanInstanceLayers();

		std::vector<VkLayerProperties> EnumerateSupportedVulkanInstanceLayers() const;
		std::vector<const char*> EnumerateRequestedVulkanInstanceLayers() const;

		void LogSupportedVulkanInstanceLayers(const std::vector<VkLayerProperties>& layers) const;
		void LogRequestedVulkanInstanceLayers(const std::vector<const char*>& layers) const;

		bool RequestedVulkanInstanceLayersSupported(
			const std::vector<VkLayerProperties>& supportedLayers,
			const std::vector<const char*>& requestedLayers) const;
#endif

		void CreateVulkanInstance();

#if defined(DEBUG) || defined(_DEBUG)
		void CreateVulkanDebugMessenger();
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void DestroyVulkanDebugMessenger();
		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageTypes,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
#endif

		void CreateVulkanWindowSurface();
		void DestroyVulkanWindowSurface();
		void CreateVulkanWindowGlfwSurface();
#ifdef EMBER_PLATFORM_WIN32
		void CreateVulkanWindowWin32Surface();
#elif EMBER_PLATFORM_LINUX
		void CreateVulkanXLibSurface();
		void CreateVulkanXCBSurface();
		void CreateVulkanWaylandSurface();
#endif

		void PickVulkanPhysicalDevice();
		std::vector<VulkanPhysicalDeviceInfo> EnumerateSupportedVulkanPhysicalDevices();

		void LogSupportedVulkanDevices(const std::vector<VulkanPhysicalDeviceInfo>& supportedDevices) const;
		void LogDeviceInfo(const VulkanPhysicalDeviceInfo& deviceInfo) const;

		bool IsPhysicalDeviceSuitable(const VulkanPhysicalDeviceInfo& deviceInfo) const;

		VkPhysicalDeviceProperties GetVulkanPhysicalDeviceProperties(VkPhysicalDevice device) const;
		VkPhysicalDeviceFeatures GetVulkanPhysicalDeviceFeatures(VkPhysicalDevice device) const;
		VulkanQueueFamilyIndices GetVulkanPhysicalDeviceQueueFamilies(VkPhysicalDevice device) const;

		VkPhysicalDeviceFeatures EnumerateRequestedDeviceFeatures() const;
		bool RequestedVulkanDeviceFeaturesSupported(
			const VkPhysicalDeviceFeatures& supportedFeatures,
			const VkPhysicalDeviceFeatures& requestedFeatures) const;

		std::vector<VkExtensionProperties> EnumerateSupportedDeviceExtensions(VkPhysicalDevice device) const;
		std::vector<const char*> EnumerateRequestedDeviceExtensions() const;

		void LogSupportedDeviceExtensions(const std::vector<VkExtensionProperties>& extensions) const;
		void LogRequestedDeviceExtensions(const std::vector<const char*>& requestedExtensions) const;

		bool RequestedVulkanDeviceExtensionsSupported(
			const std::vector<VkExtensionProperties>& supportedExtensions,
			const std::vector<const char*>& requestedExtensions) const;

		void CreateVulkanLogicalDevice();

		void PickSwapchainProperties();

		VulkanSwapchainQueryInfo QuerySwapchainSupport(VkPhysicalDevice device) const;

		VkSurfaceFormatKHR PickSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
		VkPresentModeKHR PickSwapchainPresentFormat(const std::vector<VkPresentModeKHR>& presentModes);
		VkExtent2D PickSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		void CreateSwapchain();
		void DestroySwapchain();
		void AcquireSwapchainImages();
		void CreateSwapchainImageViews();
		void DestroySwapchainImageViews();
		void ResizeSwapchain();
		void HandleSurfaceLostError();

		void CreateGraphicsPipeline();
		void CreateRenderPass();
		void CreatePipelineLayout();

		void CreateFramebuffers();
		void DestroyFramebuffers();

		void CreateCommandPools();
		void DestroyCommandPools();
		void CreateCommandBuffers();
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t swapchainImageIdx);

		void CreateSynchronizationObjects();
		void CreateFrameResourceSynchronizationObjects();
		void CreateSwapchainImageResourceSynchronizationObjects();
		void DestroySynchronizationObjects();
		void DestroyFrameResourceSynchronizationObjects();
		void DestroySwapchainImageResourceSynchronizationObjects();
		void Synchronize();

		VulkanData vulkanData;

		std::vector<VulkanFrameResources> frameRes;
		std::vector<VulkanSwapchainImageResources> swapchainImageRes;

		std::shared_ptr<VulkanGraphicsPipeline> graphicsPipeline;
		std::shared_ptr<VulkanRenderPass> renderPass;
		std::shared_ptr<VulkanPipelineLayout> pipelineLayout;

		VkClearColorValue clearColor{0.0f, 0.0f, 0.0f, 1.0f};

		Window* window{ nullptr };

		uint32_t framesInFlight{1};
		uint32_t frame{0};
		uint32_t imageIdx{0};

		SettingsVk settings;
	};

	GpuApiCtxVk* CreateGpuApiCtxVk(Window* window);

	void SetCurrentGpuApiCtxVk(GpuApiCtxVk* gpuApiCtxVk);
	GpuApiCtxVk* GetCurrentGpuApiCtxVk();

}