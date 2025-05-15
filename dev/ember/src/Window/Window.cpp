#include "Window/Window.h"
#include "Window/WindowGlfw.h"

#include "GpuApi/GpuApiCtxOgl.h"

#include <cassert>

namespace ember {

#ifdef EMBER_PLATFORM_WIN32
	static Window* CreateWindowsWindowOgl(const WindowSettings& windowSettings) {
		// 1. GLFW & OpenGL
		WindowGlfw::InitializeGlfwLibrary();
		WindowGlfw* window = new WindowGlfw(windowSettings);
		const SettingsGlfw& glfwOglSettings = GetGpuApiCtxOgl()->GetSettingsGlfw();
		window->InitializeOpenGLSpecific(glfwOglSettings);
		window->CreateWindow();
		window->MakeContextCurrent();
		WindowGlfw::LoadOpenGlFunctions();

		// 2. Win32 API
		// [TODO]

		return window;
	}
	static Window* CreateWindowsWindowVulkan(const WindowSettings& windowSettings) {
		assert(false && "Vulkan is not supported yet!");
		return nullptr;
	}
#elif EMBER_PLATFORM_LINUX
	static Window* CreateLinuxWindowOgl(const WindowSettings& windowSettings) {
		return nullptr;
	}
	static Window* CreateLinuxWindowVulkan(const WindowSettings& windowSettings) {
		return nullptr;
	}
#endif

	Window::Window(const WindowSettings& windowSettings)
		: windowSettings(windowSettings) {
	}
	Window::~Window() {
		RemoveEventRegistry();
	}

	void Window::SetEventRegistry(/* EventRegistry* eventRegistry */) {
		// TODO: set event registry implementation
	}
	void Window::RemoveEventRegistry() {
		// TODO: remove event registry implementation
	}

	const WindowSettings& Window::GetWindowSettings() const {
		return windowSettings;
	}

	std::string_view Window::GetWindowTitle() const {
		return windowSettings.windowTitle;
	}
	const Dimensions2D& Window::GetWindowDimensions() const {
		return windowSettings.windowDimensions;
	}

	uint32_t Window::GetWindowWidth() const {
		return windowSettings.windowDimensions.Width();
	}
	uint32_t Window::GetWindowHeight() const {
		return windowSettings.windowDimensions.Height();
	}

	bool Window::IsFullScreen() const {
		return windowSettings.isFullscreen;
	}
	bool Window::IsVisible() const {
		return windowSettings.isVisible;
	}

	Window* CreateWindow(const WindowSettings& windowSettings, GpuApiType gpuApiType) {
#ifdef EMBER_PLATFORM_WIN32
		// 1. On Windows we really only have one option: Win32 API.
		//    However, we can choose whether to use GLFW,
		//    or our own Win32 API Windows implementation
		if (gpuApiType == GpuApiType::OPENGL) {
			return CreateWindowsWindowOgl(windowSettings);
		} else if (gpuApiType == GpuApiType::VULKAN) {
			assert(false && "Vulkan is not supported yet!");
		} else {
			assert(false && "Invalid GPU API type id provided!");
		}
#elif EMBER_PLATFORM_LINUX
		// 2. On Linux there are two options:
		//    - X11 Window, or
		//    - Wayland Window
		//    And again, we can use GLFW, or our own implementations.
		if (gpuApiType == GpuApiType::OPENGL) {
			// TODO:
		} else if (gpuApiType == GpuApiType::VULKAN) {
			assert(false && "Vulkan is not supported yet!");
		} else {
			assert(false && "Invalid GPU API type id provided!");
		}
#else
#error "Platform is not supported!"
#endif
		// Shouldn't be reacheable!
		assert(false && "Something wrong with the defines?");
		return nullptr;
	}

}