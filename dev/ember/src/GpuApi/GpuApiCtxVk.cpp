#include "GpuApi/GpuApiCtxVk.h"

#ifdef EMBER_PLATFORM_WIN32
#include <Windows.h>
#undef CreateWindow
#undef min
#undef max
#endif
#include "Window/WindowGlfw.h"
#ifdef EMBER_PLATFORM_WIN32
#include <vulkan/vulkan_win32.h>
#endif

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <unordered_set>

namespace ember {

	static GpuApiCtxVk* currentGpuApiCtxVk{nullptr};

#ifndef NDEBUG
	VKAPI_ATTR VkResult VKAPI_CALL CreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pMessenger) {
		auto func =
			(PFN_vkCreateDebugUtilsMessengerEXT)
				vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func) {
			return func(instance, pCreateInfo, pAllocator, pMessenger);
		} else {
			throw std::runtime_error{ "Couldn't load the 'vkCreateDebugUtilsMessengerEXT' Vulkan extension function!" };
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}
	VKAPI_ATTR void VKAPI_CALL DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT messenger,
		const VkAllocationCallbacks* pAllocator) {
		auto func =
			(PFN_vkDestroyDebugUtilsMessengerEXT)
				vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func) {
			func(instance, messenger, pAllocator);
		} else {
			throw std::runtime_error{ "Couldn't load the 'vkDestroyDebugUtilsMessengerEXT' Vulkan extension function!" };
		}
	}
#endif

	bool VulkanQueueFamilyIndices::HasGraphicsQueueFamily() const {
		return graphicsQueueFamily.has_value();
	}
	bool VulkanQueueFamilyIndices::HasPresentQueueFamily() const {
		return presentQueueFamily.has_value();
	}
	bool VulkanQueueFamilyIndices::Complete() const {
		return HasGraphicsQueueFamily() && HasPresentQueueFamily();
	}

	VkInstance VulkanData::GetInstance() const {
		return instanceData.instance;
	}
	VkPhysicalDevice VulkanData::GetPhysicalDevice() const {
		return deviceData.physicalDeviceInfo.physicalDevice;
	}
	VkDevice VulkanData::GetLogicalDevice() const {
		return deviceData.logicalDevice;
	}
	VkSwapchainKHR VulkanData::GetSwapchain() const {
		return deviceData.swapchainData.swapchain;
	}

	VulkanDeviceData& VulkanData::GetDeviceData() {
		return deviceData;
	}
	const VulkanDeviceData& VulkanData::GetDeviceData() const {
		return deviceData;
	}

	VulkanSwapchainData& VulkanData::GetSwapchainData() {
		return deviceData.swapchainData;
	}
	const VulkanSwapchainData& VulkanData::GetSwapchainData() const {
		return deviceData.swapchainData;
	}

	VulkanPhysicalDeviceInfo& VulkanData::GetPhysicalDeviceInfo() {
		return deviceData.physicalDeviceInfo;
	}
	const VulkanPhysicalDeviceInfo& VulkanData::GetPhysicalDeviceInfo() const {
		return deviceData.physicalDeviceInfo;
	}

	VulkanQueueFamily& VulkanData::GetGraphicsQueueFamily() {
		return deviceData.graphicsQueueFamily;
	}
	const VulkanQueueFamily& VulkanData::GetGraphicsQueueFamily() const {
		return deviceData.graphicsQueueFamily;
	}

	VulkanQueueFamily& VulkanData::GetPresentationQueueFamily() {
		return deviceData.presentationQueueFamily;
	}
	const VulkanQueueFamily& VulkanData::GetPresentationQueueFamily() const {
		return deviceData.presentationQueueFamily;
	}

	GpuApiCtxVk::GpuApiCtxVk(const SettingsVk& settings, Window* window)
		: settings(settings), window(window) {
	}

	GpuApiType GpuApiCtxVk::GetGpuApiType() const {
		return GpuApiType::VULKAN;
	}

	void GpuApiCtxVk::Initialize() {
		EnumerateVulkanInstanceExtensions();
#if defined(DEBUG) || defined(_DEBUG)
		EnumerateVulkanInstanceLayers();
#endif
		CreateVulkanInstance();
#if defined(DEBUG) || defined(_DEBUG)
		CreateVulkanDebugMessenger();
#endif
		CreateVulkanWindowSurface();

		vulkanData.deviceData.requestedDeviceExtensions = EnumerateRequestedDeviceExtensions();
		vulkanData.deviceData.requestedDeviceLayers; // Newer Vulkan SDKs ignore this since device layers were deprecated
		// vulkanData.deviceData.requestedDeviceLayers = requestedInstanceLayers;
		vulkanData.deviceData.requestedFeatures = EnumerateRequestedDeviceFeatures();

		PickVulkanPhysicalDevice();
		CreateVulkanLogicalDevice();

		PickSwapchainProperties();
		CreateSwapchain();
		swapchainImageRes.resize(vulkanData.GetSwapchainData().swapchainImageCount);
		AcquireSwapchainImages();
		CreateSwapchainImageViews();

		CreateGraphicsPipeline();
		CreateFramebuffers();

		frameRes.resize(framesInFlight);
		CreateCommandPools();
		CreateCommandBuffers();

		CreateSynchronizationObjects();
	}
	void GpuApiCtxVk::InitializeGuiContext() {
		// TODO
	}

	void GpuApiCtxVk::Terminate() {
		Synchronize();
		DestroySynchronizationObjects();
		DestroyCommandPools();
		DestroyFramebuffers();
		graphicsPipeline->DestroyPipeline(vulkanData.GetLogicalDevice());
		renderPass->DestroyRenderPass(vulkanData.GetLogicalDevice());
		pipelineLayout->DestroyPipelineLayout(vulkanData.GetLogicalDevice());
		DestroySwapchainImageViews();
		DestroySwapchain();
		vkDestroyDevice(vulkanData.GetLogicalDevice(), nullptr);
		vkDestroySurfaceKHR(vulkanData.GetInstance(), vulkanData.surface, nullptr);
		DestroyVulkanDebugMessenger();
		vkDestroyInstance(vulkanData.GetInstance(), nullptr);
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
		vkWaitForFences(vulkanData.GetLogicalDevice(), 1, &frameRes[frame].frameFinishedFence, VK_TRUE, UINT64_MAX);
		VkResult acquireImageResult{};
		do {
			acquireImageResult = vkAcquireNextImageKHR(vulkanData.GetLogicalDevice(),
													   vulkanData.GetSwapchain(),
													   UINT64_MAX, 
													   frameRes[frame].imageAvailableSemaphore,
													   VK_NULL_HANDLE, &imageIdx);
			if (acquireImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
				OnFramebufferResize();
			} else if (acquireImageResult == VK_ERROR_SURFACE_LOST_KHR) {
				HandleSurfaceLostError();
			} else if (acquireImageResult == VK_SUBOPTIMAL_KHR) {
				break;
			} else if (acquireImageResult != VK_SUCCESS) {
				throw std::runtime_error{ "Failed to acquire the next swapchain image!" };
			}
		} while (acquireImageResult != VK_SUCCESS);

		vkResetCommandBuffer(frameRes[frame].commandBuffer, 0);
		RecordCommandBuffer(frameRes[frame].commandBuffer, imageIdx);

		// Reset the fence later, before submitting, to avoid a deadlock.
		vkResetFences(vulkanData.GetLogicalDevice(), 1, &frameRes[frame].frameFinishedFence);

		VkSemaphore waitSemaphores[]{
			frameRes[frame].imageAvailableSemaphore
		};
		VkPipelineStageFlags waitStages[]{
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
		};
		VkSemaphore signalSemaphores[]{
			swapchainImageRes[imageIdx].renderingFinishedSemaphore
		};
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &frameRes[frame].commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
		if (vkQueueSubmit(vulkanData.GetGraphicsQueueFamily().queueHandle, 1,
						  &submitInfo, frameRes[frame].frameFinishedFence) != VK_SUCCESS) {
			throw std::runtime_error{"Failed to submit a command buffer to the graphics queue!"};
		}
	}
	void GpuApiCtxVk::Present() {
		VkSemaphore signalSemaphores[]{
			swapchainImageRes[imageIdx].renderingFinishedSemaphore
		};
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		VkSwapchainKHR swapChains[] = {vulkanData.GetSwapchain()};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIdx;
		presentInfo.pResults = nullptr;
		VkResult presentResult = vkQueuePresentKHR(vulkanData.GetPresentationQueueFamily().queueHandle, &presentInfo);
		if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
			OnFramebufferResize();
		} else if (presentResult == VK_ERROR_SURFACE_LOST_KHR) {
			HandleSurfaceLostError();
		} else if (presentResult != VK_SUCCESS) {
			throw std::runtime_error{ "Failed to present a swapchain image!" };
		}
	}

	void GpuApiCtxVk::OnFramebufferResize() {
		Synchronize();
		if (!window->IsMinimized())
			ResizeSwapchain();
	}

	void GpuApiCtxVk::CreateMeshGpuResource(Mesh* mesh) {
		// TODO
	}
	void GpuApiCtxVk::UploadMeshGpuResource(Mesh* mesh) {
		// TODO
	}

	const SettingsVk& GpuApiCtxVk::GetSettingsVk() const {
		return settings;
	}

	void GpuApiCtxVk::EnumerateVulkanInstanceExtensions() {
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
		else if (windowApiType == WindowApiType::EM_XLIB) {
			assert(false && "[Vulkan (X11)] XLib Window API is not supported yet!");
		} else if (windowApiType == WindowApiType::EM_XCB) {
			assert(false && "[Vulkan (X11)] XCB Window API is not supported yet!");
		} else if (windowApiType == WindowApiType::EM_WAYLAND) {
			assert(false && "[Vulkan (Wayland)] Wayland Window API is not supported yet!");
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
			auto searchResult = supportedSet.find(requestedExtensionName);
			if (searchResult == supportedSet.end()) {
				return false;
			}
		}
		return true;
	}

#if defined(DEBUG) || defined(_DEBUG)
	void GpuApiCtxVk::EnumerateVulkanInstanceLayers() {
		std::vector<VkLayerProperties> instanceLayers = EnumerateSupportedVulkanInstanceLayers();
		std::vector<const char*> requestedInstanceLayers = EnumerateRequestedVulkanInstanceLayers();

		LogSupportedVulkanInstanceLayers(instanceLayers);
		LogRequestedVulkanInstanceLayers(requestedInstanceLayers);

		if (!RequestedVulkanInstanceLayersSupported(instanceLayers, requestedInstanceLayers)) {
			throw std::runtime_error{ "Failed to create a Vulkan Instance! Requested layers are not supported!" };
		}
		vulkanData.instanceData.layers = requestedInstanceLayers;
	}

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
		appInfo.apiVersion = VK_API_VERSION_1_3; // VK_API_VERSION_1_4

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

		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};
		PopulateDebugMessengerCreateInfo(debugMessengerCreateInfo);
		createInfo.pNext = &debugMessengerCreateInfo;
#endif

		VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkanData.instanceData.instance);
		if (result != VK_SUCCESS) {
			throw std::runtime_error{ "Failed to create a Vulkan Instance!" };
		}
		std::cout << "Vulkan Instance was successfully created!\n";
	}

#if defined(DEBUG) || defined(_DEBUG)
	void GpuApiCtxVk::CreateVulkanDebugMessenger() {
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		PopulateDebugMessengerCreateInfo(createInfo);
		if (CreateDebugUtilsMessengerEXT(
			vulkanData.instanceData.instance,
			&createInfo,
			nullptr,
			&vulkanData.instanceData.debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error{ "Failed to create a Vulkan debug messenger object!" };
		}
	}
	void GpuApiCtxVk::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo.flags = VkDebugUtilsMessengerCreateFlagsEXT{};
		createInfo.pNext = nullptr;
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = GpuApiCtxVk::VulkanDebugCallback;
		createInfo.pUserData = this;
	}
	void GpuApiCtxVk::DestroyVulkanDebugMessenger() {
		DestroyDebugUtilsMessengerEXT(
			vulkanData.instanceData.instance,
			vulkanData.instanceData.debugMessenger,
			nullptr);
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL GpuApiCtxVk::VulkanDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageTypes,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			GpuApiCtxVk* ctx = static_cast<GpuApiCtxVk*>(pUserData);
			std::cerr << "[Validation Layer]";
			switch (messageSeverity) {
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				std::cerr << "[VERBOSE]";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				std::cerr << "[INFO]";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				std::cerr << "[WARNING]";
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				std::cerr << "[ERROR]";
				break;
			default:
				std::cerr << "[UNKNOWN SEVERITY]";
				break;
			}
			switch (messageTypes) {
			case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
				std::cerr << "[GENERAL]";
				break;
			case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
				std::cerr << "[PERFORMANCE]";
				break;
			case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
				std::cerr << "[VALIDATION]";
				break;
			default:
				std::cerr << "[UNKNOWN MESSAGE TYPE]";
				break;
			}
			std::cerr << ": \n";
			std::cerr << pCallbackData->pMessage << std::endl;
		}
		return VK_FALSE;
	}
#endif

	void GpuApiCtxVk::CreateVulkanWindowSurface(){
		if (window->GetWindowType() == WindowApiType::EM_GLFW) {
			CreateVulkanWindowGlfwSurface();
		}
#ifdef EMBER_PLATFORM_WIN32
		else if (window->GetWindowType() == WindowApiType::EM_WIN32) {
			CreateVulkanWindowWin32Surface();
		}
#elif EMBER_PLATFORM_LINUX
		else if (window->GetWindowType() == WindowApiType::EM_XLIB) {
			CreateVulkanXLibSurface();
		} else if (window->GetWindowType() == WindowApiType::EM_XCB) {
			CreateVulkanXCBSurface();
		} else if (window->GetWindowType() == WindowApiType::EM_WAYLAND) {
			CreateVulkanWaylandSurface();
		}
#endif
		else {
			assert(false && "[Vulkan Window Surface] Unsupported Window API!");
		}
	}
	void GpuApiCtxVk::DestroyVulkanWindowSurface() {
		vkDestroySurfaceKHR(vulkanData.GetInstance(), vulkanData.surface, nullptr);
		vulkanData.surface = VK_NULL_HANDLE;
	}
	void GpuApiCtxVk::CreateVulkanWindowGlfwSurface() {
		WindowGlfw* windowGlfw = static_cast<WindowGlfw*>(window);
		VkResult result =
			glfwCreateWindowSurface(
				vulkanData.GetInstance(), windowGlfw->GetApiSpecificHandle(), nullptr, &vulkanData.surface);
		if (result != VK_SUCCESS) {
			throw std::runtime_error{ "[Vulkan (GLFW)] Failed to create a window surface!" };
		}
	}
#ifdef EMBER_PLATFORM_WIN32
	void GpuApiCtxVk::CreateVulkanWindowWin32Surface() {
		// TODO
	}
#elif EMBER_PLATFORM_LINUX
	void GpuApiCtxVk::CreateVulkanXLibSurface() {
		// TODO
	}
	void GpuApiCtxVk::CreateVulkanXCBSurface() {
		// TODO
	}
	void GpuApiCtxVk::CreateVulkanWaylandSurface() {
		// TODO
	}
#endif

	void GpuApiCtxVk::PickVulkanPhysicalDevice() {
		// Our goal here is to pick a discrete GPU if there are any.
		// If there are none, then we can settle with an integrated one.
		// If we don't have that either, then we throw an exception.
		std::vector<VulkanPhysicalDeviceInfo> supportedDevicesInfo = EnumerateSupportedVulkanPhysicalDevices();
		LogSupportedVulkanDevices(supportedDevicesInfo);
		
		const VulkanPhysicalDeviceInfo* pickedDeviceInfo{nullptr};
		for (const VulkanPhysicalDeviceInfo& checkDeviceInfo : supportedDevicesInfo) {
			if (IsPhysicalDeviceSuitable(checkDeviceInfo)) {
				VkPhysicalDeviceType checkDeviceType = checkDeviceInfo.deviceProperties.deviceType;
				// We pick the first available discrete GPU or
				// if there is no other choice, we settle with the first available integrated one.
				if (!pickedDeviceInfo || checkDeviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
					pickedDeviceInfo = &checkDeviceInfo;
				// The check below is to make sure that the first available discrete GPU is chosen
				// If, however, you want to choose the last available from the list, just comment the check below.
				if (checkDeviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
					break;
			}
		}
		if (!pickedDeviceInfo) {
			throw std::runtime_error{ "No suitable Vulkan device found!" };
		}
		vulkanData.deviceData.physicalDeviceInfo = *pickedDeviceInfo;
		vulkanData.deviceData.graphicsQueueFamily.queueFamilyId =
			pickedDeviceInfo->queueFamilyIds.graphicsQueueFamily.value();
		vulkanData.deviceData.presentationQueueFamily.queueFamilyId =
			pickedDeviceInfo->queueFamilyIds.presentQueueFamily.value();
	}
	void GpuApiCtxVk::LogSupportedVulkanDevices(const std::vector<VulkanPhysicalDeviceInfo>& supportedDevices) const {
		std::cout << "\n";
		std::cout << "*****************************\n";
		std::cout << " [Supported Vulkan devices]:\n";
		std::cout << "*****************************\n";
		for (const VulkanPhysicalDeviceInfo& deviceInfo : supportedDevices) {
			LogDeviceInfo(deviceInfo);
		}
		std::cout << "\n";
	}
	void GpuApiCtxVk::LogDeviceInfo(const VulkanPhysicalDeviceInfo& deviceInfo) const {
		std::cout << "[" << deviceInfo.deviceProperties.deviceName << "]\n";
	}

	bool GpuApiCtxVk::IsPhysicalDeviceSuitable(const VulkanPhysicalDeviceInfo& deviceInfo) const {
		// We only accept discrete and integrated GPUs.
		VkPhysicalDeviceType deviceType = deviceInfo.deviceProperties.deviceType;
		if (deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
			deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
			return false;
		}
		// All device features must be supported.
		if (!RequestedVulkanDeviceFeaturesSupported(
			deviceInfo.deviceFeatures, vulkanData.deviceData.requestedFeatures)) {
			return false;
		}
		// All requested queues must be present.
		if (!deviceInfo.queueFamilyIds.Complete()) {
			return false;
		}
		// All requested device extensions must be supported.
		if (!RequestedVulkanDeviceExtensionsSupported(
			deviceInfo.deviceExtensions,
			vulkanData.deviceData.requestedDeviceExtensions)) {
			return false;
		}
		// The swap chain support must be adequate.
		bool swapchainAdequate = deviceInfo.swapchainInfo.has_value() &&
			!deviceInfo.swapchainInfo.value().formats.empty() &&
			!deviceInfo.swapchainInfo.value().presentModes.empty();
		if (!swapchainAdequate) {
			return false;
		}
		return true;
	}

	std::vector<VulkanPhysicalDeviceInfo> GpuApiCtxVk::EnumerateSupportedVulkanPhysicalDevices() {
		uint32_t vulkanDeviceCount{ 0 };
		vkEnumeratePhysicalDevices(vulkanData.instanceData.instance, &vulkanDeviceCount, nullptr); 
		if (!vulkanDeviceCount) {
			throw std::runtime_error{ "No device with Vulkan support found!" };
		}
		std::vector<VkPhysicalDevice> devices(vulkanDeviceCount);
		vkEnumeratePhysicalDevices(vulkanData.instanceData.instance, &vulkanDeviceCount, devices.data());

		std::vector<VulkanPhysicalDeviceInfo> supportedDevicesInfo(vulkanDeviceCount);
		uint32_t deviceIdx{0};
		for (VkPhysicalDevice deviceHandle : devices) {
			VulkanPhysicalDeviceInfo deviceQueryInfo{};
			deviceQueryInfo.physicalDevice = deviceHandle;
			deviceQueryInfo.deviceProperties = GetVulkanPhysicalDeviceProperties(deviceHandle);
			deviceQueryInfo.deviceFeatures = GetVulkanPhysicalDeviceFeatures(deviceHandle);
			deviceQueryInfo.deviceExtensions = EnumerateSupportedDeviceExtensions(deviceHandle);
			deviceQueryInfo.queueFamilyIds = GetVulkanPhysicalDeviceQueueFamilies(deviceHandle);
			deviceQueryInfo.swapchainInfo = QuerySwapchainSupport(deviceHandle);
			supportedDevicesInfo[deviceIdx] = std::move(deviceQueryInfo);
			deviceIdx++;
		}
		return supportedDevicesInfo;
	}
	VkPhysicalDeviceProperties GpuApiCtxVk::GetVulkanPhysicalDeviceProperties(VkPhysicalDevice device) const {
		VkPhysicalDeviceProperties deviceProperties{};
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		return deviceProperties;
	}
	VkPhysicalDeviceFeatures GpuApiCtxVk::GetVulkanPhysicalDeviceFeatures(VkPhysicalDevice device) const {
		VkPhysicalDeviceFeatures deviceFeatures{};
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
		return deviceFeatures;
	}
	VulkanQueueFamilyIndices GpuApiCtxVk::GetVulkanPhysicalDeviceQueueFamilies(VkPhysicalDevice device) const {
		VulkanQueueFamilyIndices queueFamilyIds;
		uint32_t queueFamilyCount{0};
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		if (!queueFamilyCount) {
			throw std::runtime_error{ "No queue families found!" };
		}
		std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

		uint32_t queueFamilyId{0};
		for (const VkQueueFamilyProperties& queueFamilyProps : queueFamilyProperties) {
			// We want the first queue that supports graphics operations.
			if (!queueFamilyIds.graphicsQueueFamily.has_value() &&
				queueFamilyProps.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				queueFamilyIds.graphicsQueueFamily = queueFamilyId;
			}
			VkBool32 presentSupported{false};
			vkGetPhysicalDeviceSurfaceSupportKHR(device, queueFamilyId, vulkanData.surface, &presentSupported);
			// We also want the first queue that supports presentation.
			if (!queueFamilyIds.presentQueueFamily.has_value() && presentSupported) {
				queueFamilyIds.presentQueueFamily = queueFamilyId;
			}
			if (queueFamilyIds.Complete())
				break;
			queueFamilyId++;
		}
		return queueFamilyIds;
	}

	VkPhysicalDeviceFeatures GpuApiCtxVk::EnumerateRequestedDeviceFeatures() const {
		VkPhysicalDeviceFeatures requestedFeatures{};
		// The following features are just to test the check features mechanism.
		requestedFeatures.shaderFloat64 = VK_TRUE;
		requestedFeatures.shaderInt64 = VK_TRUE;
		return requestedFeatures;
	}
	bool GpuApiCtxVk::RequestedVulkanDeviceFeaturesSupported(
		const VkPhysicalDeviceFeatures& supportedFeatures,
		const VkPhysicalDeviceFeatures& requestedFeatures) const {
		const int structSize = sizeof(VkPhysicalDeviceFeatures);
		const int fieldSize = sizeof(VkBool32);
		const int fieldCount = structSize / fieldSize;

		const char* supportedFeaturesBasePtr = reinterpret_cast<const char*>(&supportedFeatures);
		const char* requestedFeaturesBasePtr = reinterpret_cast<const char*>(&requestedFeatures);
		for (int fieldIdx = 0; fieldIdx < fieldCount; fieldIdx++) {
			int offset = fieldIdx * fieldSize;
			const VkBool32* currentSupportedFeature = reinterpret_cast<const VkBool32*>(supportedFeaturesBasePtr + offset);
			const VkBool32* currentRequestedFeature = reinterpret_cast<const VkBool32*>(requestedFeaturesBasePtr + offset);
			if (*currentRequestedFeature == VK_FALSE)
				continue;
			if (*currentRequestedFeature != *currentSupportedFeature)
				return false;
		}
		return true;
	}

	std::vector<VkExtensionProperties> GpuApiCtxVk::EnumerateSupportedDeviceExtensions(VkPhysicalDevice device) const {
		uint32_t deviceExtensionCount{0};
		vkEnumerateDeviceExtensionProperties(device, 0, &deviceExtensionCount, 0);
		std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
		vkEnumerateDeviceExtensionProperties(device, 0, &deviceExtensionCount, deviceExtensions.data());
		return deviceExtensions;
	}
	std::vector<const char*> GpuApiCtxVk::EnumerateRequestedDeviceExtensions() const {
		std::vector<const char*> requestedDeviceExtensions{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		return requestedDeviceExtensions;
	}

	void GpuApiCtxVk::LogSupportedDeviceExtensions(const std::vector<VkExtensionProperties>& extensions) const
	{
		std::cout << "[Supported Vulkan device extensions]:\n";
		for (const VkExtensionProperties& extension : extensions) {
			std::cout << "\t" << extension.extensionName << "\n";
		}
		std::cout << "\n";
	}
	void GpuApiCtxVk::LogRequestedDeviceExtensions(const std::vector<const char*>& requestedExtensions) const {
		std::cout << "[Requested Vulkan device extensions]:\n";
		for (const char* extensionName : requestedExtensions) {
			std::cout << "\t" << extensionName << "\n";
		}
		std::cout << "\n";
	}

	bool GpuApiCtxVk::RequestedVulkanDeviceExtensionsSupported(
		const std::vector<VkExtensionProperties>& supportedExtensions,
		const std::vector<const char*>& requestedExtensions) const {
		// Fill in the set with the names of the supported extensions.
		std::unordered_set<std::string_view> supportedSet;
		for (const VkExtensionProperties& extension : supportedExtensions) {
			supportedSet.insert(extension.extensionName);
		}
		// Check if the names of the requested extensions are in the supported set.
		for (const char* requestedExtensionName : requestedExtensions) {
			auto searchResult = supportedSet.find(requestedExtensionName);
			if (searchResult == supportedSet.end()) {
				return false;
			}
		}
		return true;
	}

	void GpuApiCtxVk::CreateVulkanLogicalDevice() {
		// Our goal is to create a single queue for each unique queue family we picked.
		std::unordered_set<uint32_t> uniqueQueueFamilies;
		uniqueQueueFamilies.insert(vulkanData.deviceData.graphicsQueueFamily.queueFamilyId);
		uniqueQueueFamilies.insert(vulkanData.deviceData.presentationQueueFamily.queueFamilyId);

		uint32_t i{0};
		float queuePriority{1.0f};
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(uniqueQueueFamilies.size());
		for (uint32_t queueFamilyIdx : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamilyIdx;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos[i++] = std::move(queueCreateInfo);
		}

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

		deviceCreateInfo.pEnabledFeatures = &vulkanData.deviceData.requestedFeatures;
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(vulkanData.deviceData.requestedDeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = vulkanData.deviceData.requestedDeviceExtensions.data();

#if defined(DEBUG) || defined(_DEBUG)
		// Device-specific layers seems to be a deprecated feature, newer Vulkan SDKs ignore the corresponding fields.
		// However, it is said that it's still recommended to set them for compatibility.
		// We can just use the same validation layers that we used creating the Vulkan Instance.
		// Or we can just simply set the relevant fields to represent the "no layers to enable" state.
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(vulkanData.deviceData.requestedDeviceLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = vulkanData.deviceData.requestedDeviceLayers.data();
#else
		deviceCreateInfo.enabledLayerCount = 0;
		deviceCreateInfo.ppEnabledLayerNames = nullptr;
#endif

		VkResult result = vkCreateDevice(
			vulkanData.GetPhysicalDevice(), &deviceCreateInfo, nullptr, &vulkanData.deviceData.logicalDevice);
		if (result != VK_SUCCESS) {
			throw std::runtime_error{ "Failed to instantiate a Vulkan logical device!" };
		}

		// Retrieve queue handles
		// Once again, note that the queue family indices for graphics and presentation queues
		// may very well be the same. The handles we retrieve here will be the same in this case.
		vkGetDeviceQueue(
			vulkanData.GetLogicalDevice(),
			vulkanData.GetGraphicsQueueFamily().queueFamilyId,
			0,
			&vulkanData.GetGraphicsQueueFamily().queueHandle);
		vkGetDeviceQueue(
			vulkanData.GetLogicalDevice(),
			vulkanData.GetPresentationQueueFamily().queueFamilyId,
			0,
			&vulkanData.GetPresentationQueueFamily().queueHandle);
	}

	void GpuApiCtxVk::PickSwapchainProperties() {
		const VulkanSwapchainQueryInfo& swapchainInfo = vulkanData.GetPhysicalDeviceInfo().swapchainInfo.value();
		VulkanSwapchainData& swapchainData = vulkanData.GetSwapchainData();
		swapchainData.swapchainSurfaceFormat = PickSwapchainSurfaceFormat(swapchainInfo.formats);
		swapchainData.swapchainPresentMode = PickSwapchainPresentFormat(swapchainInfo.presentModes);
		swapchainData.swapchainExtent = PickSwapchainExtent(swapchainInfo.capabilities);

		uint32_t requestedSwapchainImageCount = swapchainInfo.capabilities.minImageCount + 1;
		// 'maxImageCount = 0' is a special value which means that there's no maximum value.
		if (swapchainInfo.capabilities.maxImageCount > 0 &&
			requestedSwapchainImageCount > swapchainInfo.capabilities.maxImageCount) {
			requestedSwapchainImageCount = swapchainInfo.capabilities.maxImageCount;
		}
		swapchainData.requestedSwapchainImageCount = requestedSwapchainImageCount;
	}

	VulkanSwapchainQueryInfo GpuApiCtxVk::QuerySwapchainSupport(VkPhysicalDevice device) const {
		VulkanSwapchainQueryInfo swapchainInfo{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vulkanData.surface, &swapchainInfo.capabilities);

		uint32_t surfaceFormatCount{ 0 };
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkanData.surface, &surfaceFormatCount, nullptr);
		if (surfaceFormatCount) {
			swapchainInfo.formats.resize(surfaceFormatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(
				device, vulkanData.surface, &surfaceFormatCount, swapchainInfo.formats.data());
		}

		uint32_t presentModeCount{ 0 };
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkanData.surface, &presentModeCount, nullptr);
		if (presentModeCount) {
			swapchainInfo.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				device, vulkanData.surface, &presentModeCount, swapchainInfo.presentModes.data());
		}
		return swapchainInfo;
	}

	VkSurfaceFormatKHR GpuApiCtxVk::PickSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
		for (const VkSurfaceFormatKHR& format : formats) {
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
				format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return format;
			}
		}
		return formats[0];
	}
	VkPresentModeKHR GpuApiCtxVk::PickSwapchainPresentFormat(const std::vector<VkPresentModeKHR>& presentModes) {
		for (const VkPresentModeKHR& presentMode : presentModes) {
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return presentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	VkExtent2D GpuApiCtxVk::PickSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() ||
			capabilities.currentExtent.height != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			int width = window->GetFramebufferWidth();
			int height = window->GetFramebufferHeight();
			VkExtent2D swapchainExtent{};
			swapchainExtent.width = std::clamp(
				static_cast<uint32_t>(width),
				capabilities.minImageExtent.width,
				capabilities.maxImageExtent.width);
			swapchainExtent.height = std::clamp(
				static_cast<uint32_t>(height),
				capabilities.minImageExtent.height,
				capabilities.maxImageExtent.height);
			return swapchainExtent;
		}
	}

	void GpuApiCtxVk::CreateSwapchain() {
		const VulkanDeviceData& deviceData = vulkanData.deviceData;
		const VulkanPhysicalDeviceInfo& deviceQueryInfo = vulkanData.deviceData.physicalDeviceInfo;
		const VulkanSwapchainQueryInfo& swapchainInfo = deviceQueryInfo.swapchainInfo.value();
		VulkanSwapchainData& swapchainData = vulkanData.GetSwapchainData();

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = vulkanData.surface;
		createInfo.minImageCount = swapchainData.requestedSwapchainImageCount;
		createInfo.imageFormat = swapchainData.swapchainSurfaceFormat.format;
		createInfo.imageColorSpace = swapchainData.swapchainSurfaceFormat.colorSpace;
		createInfo.imageExtent = swapchainData.swapchainExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		// Sharing swap chain images between different queue families
		uint32_t graphicsQeueuFamilyId = deviceQueryInfo.queueFamilyIds.graphicsQueueFamily.value();
		uint32_t presentQeueuFamilyId = deviceQueryInfo.queueFamilyIds.presentQueueFamily.value();
		uint32_t queueFamilyIds[] = { graphicsQeueuFamilyId, presentQeueuFamilyId };
		if (graphicsQeueuFamilyId != presentQeueuFamilyId) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIds;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		createInfo.preTransform = swapchainInfo.capabilities.currentTransform; // means no transform
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = swapchainData.swapchainPresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = vulkanData.GetSwapchain();
		if (vkCreateSwapchainKHR(deviceData.logicalDevice, &createInfo,
								 nullptr, &swapchainData.swapchain) != VK_SUCCESS) {
			throw std::runtime_error{ "Failed to create a swap chain" };
		}
		vkGetSwapchainImagesKHR(deviceData.logicalDevice, swapchainData.swapchain,
								&swapchainData.swapchainImageCount, nullptr);
	}
	void GpuApiCtxVk::DestroySwapchain() {
		vkDestroySwapchainKHR(vulkanData.GetLogicalDevice(), vulkanData.GetSwapchain(), nullptr);
		vulkanData.deviceData.swapchainData.swapchain = VK_NULL_HANDLE;
	}
	void GpuApiCtxVk::AcquireSwapchainImages() {
		// Now we can acquire handles to the swap chain images.
		// Remember that we only specified the minimum number of swap chain images,
		// but the implementation is free to instantiate more, so we have to query that as well.
		VulkanDeviceData& deviceData = vulkanData.GetDeviceData();
		VulkanSwapchainData& swapchainData = deviceData.swapchainData;

		std::vector<VkImage> swapchainImagesTmp(swapchainData.swapchainImageCount);
		vkGetSwapchainImagesKHR(deviceData.logicalDevice, swapchainData.swapchain,
								&swapchainData.swapchainImageCount, swapchainImagesTmp.data());

		for (uint32_t idx = 0; idx < swapchainData.swapchainImageCount; idx++) {
			swapchainImageRes[idx].image = swapchainImagesTmp[idx];
		}
	}
	void GpuApiCtxVk::CreateSwapchainImageViews() {
		VulkanDeviceData& deviceData = vulkanData.GetDeviceData();
		VulkanSwapchainData& swapchainData = vulkanData.GetSwapchainData();
		for (uint32_t idx = 0; idx < swapchainData.swapchainImageCount; idx++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapchainImageRes[idx].image;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapchainData.swapchainSurfaceFormat.format;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			if (vkCreateImageView(deviceData.logicalDevice, &createInfo, nullptr,
								  &swapchainImageRes[idx].imageView) != VK_SUCCESS) {
				throw std::runtime_error{ "Failed to create an image view for a swap chain image!" };
			}
		}
	}
	void GpuApiCtxVk::DestroySwapchainImageViews() {
		VulkanDeviceData& deviceData = vulkanData.GetDeviceData();
		VulkanSwapchainData& swapchainData = vulkanData.GetSwapchainData();
		std::for_each(swapchainImageRes.begin(), swapchainImageRes.end(),
			[&](VulkanSwapchainImageResources& res) {
				vkDestroyImageView(deviceData.logicalDevice, res.imageView, nullptr);
				res.imageView = VK_NULL_HANDLE;
			});
	}
	void GpuApiCtxVk::ResizeSwapchain() {
		DestroySwapchainImageResourceSynchronizationObjects();
		DestroyFramebuffers();
		DestroySwapchainImageViews();
		DestroySwapchain();

		vulkanData.deviceData.physicalDeviceInfo.swapchainInfo =
			QuerySwapchainSupport(vulkanData.GetPhysicalDevice());
		PickSwapchainProperties();
		CreateSwapchain();
		swapchainImageRes.resize(vulkanData.GetSwapchainData().swapchainImageCount);
		AcquireSwapchainImages();
		CreateSwapchainImageViews();
		CreateFramebuffers();
		CreateSwapchainImageResourceSynchronizationObjects();
	}
	void GpuApiCtxVk::HandleSurfaceLostError() {
		Synchronize();
		DestroyVulkanWindowSurface();
		CreateVulkanWindowSurface();
		if (!window->IsMinimized())
			ResizeSwapchain();
	}

	void GpuApiCtxVk::CreateGraphicsPipeline() {
		graphicsPipeline = std::make_shared<VulkanGraphicsPipeline>();
		graphicsPipeline->SetAttachmentCount(1);

		VulkanShaderModule vertexShaderModule{};
		std::filesystem::path vshaderRelPath = std::filesystem::path{ "resource/shaders/spirv/vshader.spv" }.make_preferred();
		vertexShaderModule.shaderPath = std::filesystem::current_path() / vshaderRelPath;
		vertexShaderModule.entryPoint = std::string{ "main" };
		vertexShaderModule.shaderType = SHADER_TYPE::VERTEX_SHADER;
		VulkanShaderFactory::CreateShaderModule(
			vertexShaderModule, vulkanData.deviceData.logicalDevice);
		graphicsPipeline->SetVertexShaderModule(vertexShaderModule);

		VulkanShaderModule fragmentShaderModule;
		std::filesystem::path fshaderRelPath = std::filesystem::path{ "resource/shaders/spirv/fshader.spv" }.make_preferred();
		fragmentShaderModule.shaderPath = std::filesystem::current_path() / fshaderRelPath;
		fragmentShaderModule.entryPoint = std::string{ "main" };
		fragmentShaderModule.shaderType = SHADER_TYPE::FRAGMENT_SHADER;
		VulkanShaderFactory::CreateShaderModule(
			fragmentShaderModule, vulkanData.deviceData.logicalDevice);
		graphicsPipeline->SetFragmentShaderModule(fragmentShaderModule);

		graphicsPipeline->AddDynamicState(VK_DYNAMIC_STATE_VIEWPORT);
		graphicsPipeline->AddDynamicState(VK_DYNAMIC_STATE_SCISSOR);

		// TODO: vertex input layout
		graphicsPipeline->SetPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

		VkExtent2D swapchainExtent = vulkanData.GetSwapchainData().swapchainExtent;
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchainExtent.width);
		viewport.height = static_cast<float>(swapchainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		graphicsPipeline->SetViewport(viewport);

		VkRect2D scissors{};
		scissors.offset = VkOffset2D{ 0, 0 };
		scissors.extent = swapchainExtent;
		graphicsPipeline->SetScissors(scissors);

		VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
		graphicsPipeline->SetPolygonMode(polygonMode);
		VkCullModeFlagBits cullMode = VK_CULL_MODE_BACK_BIT;
		graphicsPipeline->SetCullMode(cullMode);
		VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		graphicsPipeline->SetFrontFace(frontFace);
		graphicsPipeline->SetLineWidth(1.0f);

		graphicsPipeline->DisableMultisampling();
		graphicsPipeline->SetDefaultBlendingAttachmentState(false, 0);

		CreateRenderPass();
		graphicsPipeline->SetRenderPass(renderPass);
		CreatePipelineLayout();
		graphicsPipeline->SetPipelineLayout(pipelineLayout);

		graphicsPipeline->CreatePipeline(vulkanData.GetLogicalDevice());

		VulkanShaderFactory::DestroyShaderModule(vertexShaderModule, vulkanData.deviceData.logicalDevice);
		VulkanShaderFactory::DestroyShaderModule(fragmentShaderModule, vulkanData.deviceData.logicalDevice);
	}
	void GpuApiCtxVk::CreateRenderPass() {
		renderPass = std::make_shared<VulkanRenderPass>();
		renderPass->SetAttachmentCount(1);
		VkFormat colorAttachmentFormat = vulkanData.GetSwapchainData().swapchainSurfaceFormat.format;
		renderPass->SetPresentRenderTargetColorAttachment(colorAttachmentFormat, 0);

		renderPass->SetSubpassCount(1);
		renderPass->SetSubpassColorAttachmentCount(0, 1);
		renderPass->SetSubpassColorAttachmentReference(0, 0, 0);

		renderPass->CreateRenderPass(vulkanData.GetLogicalDevice());
	}
	void GpuApiCtxVk::CreatePipelineLayout() {
		pipelineLayout = std::make_shared<VulkanPipelineLayout>();
		pipelineLayout->CreatePipelineLayout(vulkanData.GetLogicalDevice());
	}

	void GpuApiCtxVk::CreateFramebuffers() {
		const VulkanSwapchainData& swapchainData = vulkanData.GetSwapchainData();
		uint32_t imgIdx{0};
		for (VulkanSwapchainImageResources& imageRes : swapchainImageRes) {
			imageRes.framebuffer.SetFramebufferSize(
				swapchainData.swapchainExtent.width,
				swapchainData.swapchainExtent.height);
			imageRes.framebuffer.SetRenderPass(renderPass);
			imageRes.framebuffer.SetAttachmentCount(1);
			imageRes.framebuffer.SetAttachment(imageRes.imageView, 0);
			imageRes.framebuffer.CreateFramebuffer(vulkanData.GetLogicalDevice());
		}
	}
	void GpuApiCtxVk::DestroyFramebuffers() {
		for (VulkanSwapchainImageResources& imageRes : swapchainImageRes) {
			imageRes.framebuffer.DestroyFramebuffer(vulkanData.GetLogicalDevice());
			imageRes.framebuffer.ClearAttachments();
		}
	}

	void GpuApiCtxVk::CreateCommandPools() {
		VulkanQueueFamily& graphicsQueueFamily = vulkanData.GetGraphicsQueueFamily();
		VkCommandPoolCreateInfo graphicsCommandPoolInfo{};
		graphicsCommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		graphicsCommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		graphicsCommandPoolInfo.queueFamilyIndex = graphicsQueueFamily.queueFamilyId;
		if (vkCreateCommandPool(vulkanData.GetLogicalDevice(),
								&graphicsCommandPoolInfo,
								nullptr,
								&graphicsQueueFamily.commandPool) != VK_SUCCESS) {
			throw std::runtime_error{"Failed to create a graphics command pool!"};
		}
	}
	void GpuApiCtxVk::DestroyCommandPools() {
		VulkanQueueFamily& graphicsQueueFamily = vulkanData.GetGraphicsQueueFamily();
		vkDestroyCommandPool(vulkanData.GetLogicalDevice(), graphicsQueueFamily.commandPool, nullptr);
	}
	void GpuApiCtxVk::CreateCommandBuffers() {
		VulkanQueueFamily& graphicsQueueFamily = vulkanData.GetGraphicsQueueFamily();

		VkCommandBufferAllocateInfo commandBufferInfo{};
		commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferInfo.commandPool = graphicsQueueFamily.commandPool;
		commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferInfo.commandBufferCount = 1;
		for (uint32_t idx = 0; idx < framesInFlight; idx++) {
			if (vkAllocateCommandBuffers(vulkanData.GetLogicalDevice(),
										 &commandBufferInfo,
										 &frameRes[idx].commandBuffer) != VK_SUCCESS) {
				throw std::runtime_error{ "Failed to allocate a graphics command buffer!" };
			}
		}
	}
	void GpuApiCtxVk::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t swapchainImageIdx) {
		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS) {
			throw std::runtime_error{ "Failed to start a command buffer!" };
		}

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = renderPass->GetRenderPass();
		renderPassBeginInfo.framebuffer = swapchainImageRes[swapchainImageIdx].framebuffer.GetFramebuffer();
		renderPassBeginInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		renderPassBeginInfo.renderArea.extent = vulkanData.GetSwapchainData().swapchainExtent;

		// But OpenGL outputs this color "right" by default.
		// What's the difference then?
		clearColor = VkClearColorValue{
			215.0f / 255.0f, // std::pow(215.0f / 255.0f, 2.2f),
			153.0f / 255.0f, // std::pow(153.0f / 255.0f, 2.2f),
			33.0f / 255.0f, // std::pow(33.0f / 255.0f, 2.2f),
			1.0f
		};
		VkClearValue clearValue{clearColor};
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearValue;

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->GetPipeline());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(vulkanData.GetSwapchainData().swapchainExtent.width);
		viewport.height = static_cast<float>(vulkanData.GetSwapchainData().swapchainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		VkRect2D scissors{};
		scissors.offset = VkOffset2D{0, 0};
		scissors.extent = vulkanData.GetSwapchainData().swapchainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissors);

		// The draw call
		// VkBuffer vertexBuffers[] = { dynamicVertexBuffer->GetBufferHandle() };
		// VkBuffer vertexBuffers[] = { staticVertexBuffer->GetBufferHandle() };
		// VkDeviceSize offsets[] = { 0 };
		// vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		// vkCmdDraw(commandBuffer, staticVertexBuffer->GetVertexCount(), 1, 0, 0);
		// vkCmdDraw(commandBuffer, vertexBuffer->GetVertexCount(), 1, 0, 0);
		// vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);
		vkCmdDraw(commandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffer);
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error{ "Failed to end a command buffer!" };
		}
	}

	void GpuApiCtxVk::CreateSynchronizationObjects() {
		CreateFrameResourceSynchronizationObjects();
		CreateSwapchainImageResourceSynchronizationObjects();
	}
	void GpuApiCtxVk::CreateFrameResourceSynchronizationObjects() {
		for (uint32_t i = 0; i < framesInFlight; i++) {
			VkSemaphoreCreateInfo semaphoreCreateInfo{};
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			if (vkCreateSemaphore(vulkanData.GetLogicalDevice(), &semaphoreCreateInfo,
				nullptr, &frameRes[i].imageAvailableSemaphore) != VK_SUCCESS) {
				throw std::runtime_error{ "Failed to create an 'imageAvailableSemaphore' semaphore!" };
			}
			VkFenceCreateInfo fenceCreateInfo{};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			if (vkCreateFence(vulkanData.GetLogicalDevice(), &fenceCreateInfo,
				nullptr, &frameRes[i].frameFinishedFence) != VK_SUCCESS) {
				throw std::runtime_error{ "Failed to create a 'frameFinishedFence' fence!" };
			}
		}
	}
	void GpuApiCtxVk::CreateSwapchainImageResourceSynchronizationObjects() {
		for (uint32_t i = 0; i < vulkanData.GetSwapchainData().swapchainImageCount; i++) {
			VkSemaphoreCreateInfo semaphoreCreateInfo{};
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			if (vkCreateSemaphore(vulkanData.GetLogicalDevice(), &semaphoreCreateInfo,
				nullptr, &swapchainImageRes[i].renderingFinishedSemaphore) != VK_SUCCESS) {
				throw std::runtime_error{ "Failed to create a 'renderingFinishedSemaphore' semaphore!" };
			}
		}
	}
	void GpuApiCtxVk::DestroySynchronizationObjects() {
		DestroyFrameResourceSynchronizationObjects();
		DestroySwapchainImageResourceSynchronizationObjects();
	}
	void GpuApiCtxVk::DestroyFrameResourceSynchronizationObjects() {
		for (uint32_t i = 0; i < framesInFlight; i++) {
			vkDestroySemaphore(vulkanData.GetLogicalDevice(), frameRes[i].imageAvailableSemaphore, nullptr);
			vkDestroyFence(vulkanData.GetLogicalDevice(), frameRes[i].frameFinishedFence, nullptr);
		}
	}
	void GpuApiCtxVk::DestroySwapchainImageResourceSynchronizationObjects() {
		for (uint32_t i = 0; i < vulkanData.GetSwapchainData().swapchainImageCount; i++) {
			vkDestroySemaphore(vulkanData.GetLogicalDevice(), swapchainImageRes[i].renderingFinishedSemaphore, nullptr);
		}
	}
	void GpuApiCtxVk::Synchronize() {
		vkDeviceWaitIdle(vulkanData.GetLogicalDevice());
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