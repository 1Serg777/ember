#include "Window/Window.h"
#include "Window/WindowGlfw.h"

#include <cassert>

namespace ember {

#ifdef EMBER_PLATFORM_WIN32
	static Window* CreateWindowsWindow(const WindowSettings& windowSettings) {
		if (windowSettings.type == WindowApiType::EM_GLFW) {
			// 1. GLFW API
			WindowGlfw* window = new WindowGlfw(windowSettings);
			return window;
		} else if (windowSettings.type == WindowApiType::EM_WIN32) {
			// 2. Win32 API
			// TODO: implementation
			assert(false && "[Window] Win32 Window API is not supported yet!");
			return nullptr;
		} else {
			assert(false && "[Window] Unsupported Window API!");
			return nullptr;
		}
	}
#elif EMBER_PLATFORM_LINUX
	static Window* CreateLinuxWindow(const WindowSettings& windowSettings) {
		if (windowSettings.type == WindowApiType::EM_GLFW) {
			// 1. GLFW API
			WindowGlfw* window = new WindowGlfw(windowSettings);
			return window;
		} else if (windowSettings.type == WindowApiType::EM_WAYLAND) {
			// 2. Wayland API
			// TODO: implementation
			assert(false && "[Window] Wayland Window API is not supported yet!");
			return nullptr;
		} else if (windowSettings.type == WindowApiType::EM_X11) {
			// 3. X11 API
			// TODO: implementation
			assert(false && "[Window] X11 Window API is not supported yet!");
			return nullptr;
		} else {
			assert(false && "[Window] Unsupported Window API!");
			return nullptr;
		}
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

	WindowApiType Window::GetWindowType() const {
		return windowSettings.type;
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

	Window* CreateWindow(const WindowSettings& windowSettings) {
#ifdef EMBER_PLATFORM_WIN32
		// 1. On Windows we really only have one option: Win32 API.
		//    However, we can choose whether to use the GLFW abstraction,
		//    or our own Win32 Window API implementation
		return CreateWindowsWindow(windowSettings);
#elif EMBER_PLATFORM_LINUX
		// 2. On Linux there are two options:
		//    - X11 Window, or
		//    - Wayland Window
		//    And again, we can either use the GLFW abstractions, or our own implementations.
		return CreateLinuxWindow(windowSettings);
#else
#error "Platform is not supported!"
#endif
		// Shouldn't be reacheable!
		assert(false && "[Window] Something wrong with the EMBER_PLATFORM defines?");
		return nullptr;
	}

	void InitializeWindowLibrary(WindowApiType windowApiType) {
		if (windowApiType == WindowApiType::EM_GLFW) {
			WindowGlfw::InitializeGlfwLibrary();
		}
	}
	void TerminateWindowLibrary(WindowApiType windowApiType) {
		if (windowApiType == WindowApiType::EM_GLFW) {
			WindowGlfw::TerminateGlfwLibrary();
		}
	}
}