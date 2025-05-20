#include "GpuApi/GpuApiCtxVk.h"

#include <Windows.h>
#undef CreateWindow
#include "Window/WindowGlfw.h"
#include <vulkan/vulkan_win32.h>

#include <iostream>
#include <stdexcept>
#include <unordered_set>

namespace ember {

	static GpuApiCtxVk* currentGpuApiCtxVk{nullptr};

	GpuApiCtxVk::GpuApiCtxVk(const SettingsVk& settings, Window* window)
		: settings(settings), window(window) {
	}

	GpuApiType GpuApiCtxVk::GetGpuApiType() const {
		return GpuApiType::VULKAN;
	}

	void GpuApiCtxVk::Initialize() {
		InitializeVulkanInstance(window);
	}
	void GpuApiCtxVk::InitializeGuiContext() {
		// TODO
	}

	void GpuApiCtxVk::Terminate() {
		vkDestroyInstance(vulkanData.instanceData.instance, nullptr);
	}
	void GpuApiCtxVk::TerminateGuiContext() {
		// TODO
	}

	void GpuApiCtxVk::OnMakeCurrent() {
		// TODO
	}
	void GpuApiCtxVk::OnMakeNonCurrent() {
		// TODO
	}

	void GpuApiCtxVk::OnFrameBegin() {
		// TODO
	}
	void GpuApiCtxVk::OnFrameEnd() {
		// TODO
	}
	void GpuApiCtxVk::DrawFrame() {
		// TODO
	}
	void GpuApiCtxVk::Present() {
		// TODO
	}

	const SettingsVk& GpuApiCtxVk::GetSettingsVk() const {
		return settings;
	}

	void GpuApiCtxVk::InitializeVulkanInstance(Window* window) {
		std::vector<VkExtensionProperties> instanceExtensions =
			EnumerateSupportedVulkanInstanceExtensions();
		std::vector<const char*> requestedInstanceExtensions =
			EnumerateRequestedVulkanInstanceExtensions(window->GetWindowType());

		LogSupportedVulkanInstanceExtensions(instanceExtensions);
		LogRequestedVulkanInstanceExtensions(requestedInstanceExtensions);

		if (!RequestedVulkanInstanceExtensionsSupported(instanceExtensions, requestedInstanceExtensions)) {
			throw std::runtime_error{ "Failed to create a Vulkan Instance! Requested extensions are not supported!" };
		}
		vulkanData.instanceData.extensions = requestedInstanceExtensions;

#if defined(DEBUG) || defined(_DEBUG)
		std::vector<VkLayerProperties> instanceLayers = EnumerateSupportedVulkanInstanceLayers();
		std::vector<const char*> requestedInstanceLayers = EnumerateRequestedVulkanInstanceLayers();

		LogSupportedVulkanInstanceLayers(instanceLayers);
		LogRequestedVulkanInstanceLayers(requestedInstanceLayers);

		if (!RequestedVulkanInstanceLayersSupported(instanceLayers, requestedInstanceLayers)) {
			throw std::runtime_error{ "Failed to create a Vulkan Instance! Requested layers are not supported!" };
		}
		vulkanData.instanceData.layers = requestedInstanceLayers;
#endif

		CreateVulkanInstance();
	}

	std::vector<VkExtensionProperties> GpuApiCtxVk::EnumerateSupportedVulkanInstanceExtensions() const {
		// Enumerate supported Vulkan Instance extensions
		uint32_t vkInstanceExtCount{0};
		vkEnumerateInstanceExtensionProperties(nullptr, &vkInstanceExtCount, nullptr);
		if (!vkInstanceExtCount) {
			throw std::runtime_error{ "[Vulkan] Can't initialize Vulkan: no extensions available!" };
		}
		std::vector<VkExtensionProperties> exts(vkInstanceExtCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &vkInstanceExtCount, exts.data());
		return exts;
	}
	std::vector<const char*> GpuApiCtxVk::EnumerateRequestedVulkanInstanceExtensions(WindowApiType windowApiType) const {
		std::vector<const char*> requestedExtensions;
		if (windowApiType == WindowApiType::EM_GLFW) {
			uint32_t glfwExtensionCount{0};
			const char** instanceExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
			if (!glfwExtensionCount) {
				throw std::runtime_error{ "[Vulkan (Glfw)] Couldn't retrieve GLFW-required Vulkan extensions!" };
			}
			requestedExtensions.resize(glfwExtensionCount);
			for (uint32_t idx = 0; idx < glfwExtensionCount; idx++) {
				requestedExtensions[idx] = instanceExtensionNames[idx];
			}
		}
#ifdef EMBER_PLATFORM_WIN32
		else if (windowApiType == WindowApiType::EM_WIN32) {
			requestedExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
			requestedExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		}
#elif EMBER_PLATFORM_LINUX
		else if (windowApiType == WindowApiType::WAYLAND) {
			assert(false && "[Vulkan (Wayland)] Wayland Window API is not supported yet!");
			return nullptr;
		} else if (windowApiType == WindowApiType::X11) {
			assert(false && "[Vulkan (X11)] X11 Window API is not supported yet!");
			return nullptr;
		}
#endif
		else {
			assert(false && "[Vulkan] Unsupported Window API!");
		}
		// Debug extensions
#if defined(DEBUG) || defined(_DEBUG)
		requestedExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
		return requestedExtensions;
	}

	void GpuApiCtxVk::LogSupportedVulkanInstanceExtensions(const std::vector<VkExtensionProperties>& extensions) const {
		std::cout << "[" << extensions.size() << " supported Vulkan Instance extensions]:\n";
		for (const VkExtensionProperties& extension : extensions) {
			std::cout << "\t" << extension.extensionName << "\n";
		}
		std::cout << "\n";
	}
	void GpuApiCtxVk::LogRequestedVulkanInstanceExtensions(const std::vector<const char*>& extensions) const {
		std::cout << "[Requested Vulkan Instance extensions]:\n";
		for (const char* extensionName : extensions) {
			std::cout << "\t" << extensionName << "\n";
		}
		std::cout << "\n";
	}

	bool GpuApiCtxVk::RequestedVulkanInstanceExtensionsSupported(
		const std::vector<VkExtensionProperties>& supportedExtensions,
		const std::vector<const char*>& requestedExtensions) const {
		// Fill in the set with the names of supported extensions.
		std::unordered_set<std::string_view> supportedSet;
		for (const VkExtensionProperties& extension : supportedExtensions) {
			supportedSet.insert(extension.extensionName);
		}
		// Check if the names of the requested extensions are in the supported set.
		for (const char* requestedExtensionName : requestedExtensions) {
			std::string_view extName{requestedExtensionName};
			auto searchResult = supportedSet.find(extName);
			if (searchResult == supportedSet.end()) {
				return false;
			}
		}
		return true;
	}

#if defined(DEBUG) || defined(_DEBUG)
	std::vector<VkLayerProperties> GpuApiCtxVk::EnumerateSupportedVulkanInstanceLayers() const {
		uint32_t vulkanInstanceLayerCount{0};
		vkEnumerateInstanceLayerProperties(&vulkanInstanceLayerCount, nullptr);
		if (!vulkanInstanceLayerCount) {
			throw std::runtime_error{ "[Vulkan] Can't initialize Vulkan: no layers available!" };
		}
		std::vector<VkLayerProperties> layers(vulkanInstanceLayerCount);
		vkEnumerateInstanceLayerProperties(&vulkanInstanceLayerCount, layers.data());
		return layers;
	}
	std::vector<const char*> GpuApiCtxVk::EnumerateRequestedVulkanInstanceLayers() const {
		std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
		return validationLayers;
	}

	void GpuApiCtxVk::LogSupportedVulkanInstanceLayers(const std::vector<VkLayerProperties>& layers) const {
		std::cout << "[" << layers.size() << " supported Vulkan Instance layers]:\n";
		for (const VkLayerProperties& layer : layers) {
			std::cout << "\t" << layer.layerName << "\n";
		}
		std::cout << "\n";
	}
	void GpuApiCtxVk::LogRequestedVulkanInstanceLayers(const std::vector<const char*>& layers) const {
		std::cout << "[Requested Vulkan Instance layers]:\n";
		for (const char* layerName : layers) {
			std::cout << "\t" << layerName << "\n";
		}
		std::cout << "\n";
	}

	bool GpuApiCtxVk::RequestedVulkanInstanceLayersSupported(
		const std::vector<VkLayerProperties>& supportedLayers,
		const std::vector<const char*>& requestedLayers) const {
		// Fill in the set with the names of supported layers.
		std::unordered_set<std::string_view> supportedSet;
		for (const VkLayerProperties& layer : supportedLayers) {
			supportedSet.insert(layer.layerName);
		}
		// Check if the names of the requested layers are in the supported set.
		for (const char* requestedLayerName : requestedLayers) {
			std::string_view layerName{requestedLayerName};
			auto searchResult = supportedSet.find(requestedLayerName);
			if (searchResult == supportedSet.end()) {
				return false;
			}
		}
		return true;
	}
#endif


	void GpuApiCtxVk::CreateVulkanInstance() {
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Ember";
		appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		appInfo.pEngineName = "Ember Engine";
		appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_4;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(vulkanData.instanceData.extensions.size());
		createInfo.ppEnabledExtensionNames = vulkanData.instanceData.extensions.data();

#if defined(NDEBUG) || defined(_NDEBUG)
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
#else
		createInfo.enabledLayerCount = static_cast<uint32_t>(vulkanData.instanceData.layers.size());
		createInfo.ppEnabledLayerNames = vulkanData.instanceData.layers.data();

		// We need this to catch errors when calling
		// vkCreateInstance(...) and vkDestroyInstance(...) functions.

		// VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
		// PopulateDebugMessengerCreateInfo(debugMessengerCreateInfo);
		// createInfo.pNext = &debugMessengerCreateInfo;
#endif

		VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkanData.instanceData.instance);
		if (result != VK_SUCCESS) {
			throw std::runtime_error{ "Failed to create a Vulkan Instance!" };
		}
		std::cout << "Vulkan Instance was successfully created!\n";
	}

	GpuApiCtxVk* CreateGpuApiCtxVk(Window* window) {
		SettingsVk defaultSettings{};
		GpuApiCtxVk* vkCtx = new GpuApiCtxVk(defaultSettings, window);
		return vkCtx; // The ownership is transferred to the caller!
	}

	void SetCurrentGpuApiCtxVk(GpuApiCtxVk* gpuApiCtxVk) {
		if (currentGpuApiCtxVk) {
			currentGpuApiCtxVk->OnMakeNonCurrent();
		}
		gpuApiCtxVk->OnMakeCurrent();
		currentGpuApiCtxVk = gpuApiCtxVk;
	}
	GpuApiCtxVk* GetCurrentGpuApiCtxVk() {
		return currentGpuApiCtxVk;
	}

}