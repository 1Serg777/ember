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

	void Window::SetEventRegistry(EventRegistry* eventRegistry) {
		this->eventRegistry = eventRegistry;
	}
	void Window::RemoveEventRegistry() {
		this->eventRegistry = nullptr;
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

	uint32_t Window::GetFramebufferWidth() const {
		return windowSettings.framebufferDimensions.Width();
	}
	uint32_t Window::GetFramebufferHeight() const {
		return windowSettings.framebufferDimensions.Height();
	}

	bool Window::IsFullScreen() const {
		return windowSettings.isFullscreen;
	}
	bool Window::IsVisible() const {
		return windowSettings.isVisible;
	}
	bool Window::IsMinimized() const {
		return isMinimized;
	}

	WindowApiType ChooseWindowApi(const CmdLineArgs& cmdLineArgs) {
		WindowApiType windowApiType = WindowApiType::EM_GLFW;
		if (cmdLineArgs.HasOption(cmdopt::windowApiOpt)) {
			// --windowapi[=windowapiname], where
			//   windowapiname is one of
			//   1) glfw
			//   2) win32
			//   3) xlib (x11)
			//   4) xcb (x11)
			//   5) wayland
			const Opt& opt = cmdLineArgs.GetOpt(cmdopt::windowApiOpt);
			std::string_view value = opt.GetValue().GetString();
			if (value == cmdopt::windowApiGlfwVal) {
				windowApiType = WindowApiType::EM_GLFW;
			}
#ifdef EMBER_PLATFORM_WIN32
			else if (value == cmdopt::windowApiWin32Val) {
				windowApiType = WindowApiType::EM_WIN32;
			}
#elif EMBER_PLATFORM_LINUX
			else if (value == cmdopt::windowApiXlibVal) {
				windowApiType = WindowApiType::EM_XLIB;
			} else if (value == cmdopt::windowApiXcbVal) {
				windowApiType = WindowApiType::EM_XCB;
			} else if (value == cmdopt::windowApiWaylandVal) {
				windowApiType = WindowApiType::EM_WAYLAND;
			}
#else
#error "Platform is not supported!"
#endif
			// At this point we're 100% sure that the options passed are correct.
			// In other words, the check I was trying to do below has already been done in the parser.
			/*
			else {
				std::cerr << "Unknown Window API type passed in the command line arguments: "
			}
			*/
		}
		return windowApiType;
	}
	WindowSettings ChooseWindowSettings(const CmdLineArgs& cmdLineArgs) {
		WindowSettings windowSettings{};
		windowSettings.windowDimensions = ChooseWindowDimensions(cmdLineArgs);
		windowSettings.minSizeLimit = ChooseWindowMinDimensions(cmdLineArgs);
		windowSettings.type = ChooseWindowApi(cmdLineArgs);
		windowSettings.isFullscreen = ChooseWindowFullScreenMode(cmdLineArgs);
		windowSettings.isVisible = ChooseWindowVisibility(cmdLineArgs);
		windowSettings.isResizable = ChooseWindowResizeability(cmdLineArgs);
		return windowSettings;
	}
	Dimensions2D ChooseWindowDimensions(const CmdLineArgs& cmdLineArgs) {
		uint32_t windowWidth{1920};
		uint32_t windowHeight{1080};
		if (cmdLineArgs.HasOption(cmdopt::windowWidthOpt)) {
			const Opt& opt = cmdLineArgs.GetOpt(cmdopt::windowWidthOpt);
			windowWidth = static_cast<uint32_t>(opt.GetValue().GetInt());
		}
		if (cmdLineArgs.HasOption(cmdopt::windowHeightOpt)) {
			const Opt& opt = cmdLineArgs.GetOpt(cmdopt::windowHeightOpt);
			windowHeight = static_cast<uint32_t>(opt.GetValue().GetInt());
		}
		return Dimensions2D{windowWidth, windowHeight};
	}
	Dimensions2D ChooseWindowMinDimensions(const CmdLineArgs& cmdLineArgs) {
		uint32_t windowMinWidth{320};
		uint32_t windowMinHeight{240};
		if (cmdLineArgs.HasOption(cmdopt::windowMinWidthOpt)) {
			const Opt& opt = cmdLineArgs.GetOpt(cmdopt::windowMinWidthOpt);
			windowMinWidth = static_cast<uint32_t>(opt.GetValue().GetInt());
		}
		if (cmdLineArgs.HasOption(cmdopt::windowMinHeightOpt)) {
			const Opt& opt = cmdLineArgs.GetOpt(cmdopt::windowMinHeightOpt);
			windowMinHeight = static_cast<uint32_t>(opt.GetValue().GetInt());
		}
		return Dimensions2D{windowMinWidth, windowMinHeight};
	}
	bool ChooseWindowFullScreenMode(const CmdLineArgs& cmdLineArgs) {
		// The argument can be one of the following:
		// --fullscreen=on
		// --fullscreen=off (default)
		bool isFullScreen{false};
		if (cmdLineArgs.HasOption(cmdopt::fullscreenOpt)) {
			const Opt& opt = cmdLineArgs.GetOpt(cmdopt::fullscreenOpt);
			std::string_view value = opt.GetValue().GetString();
			if (value == "on") {
				isFullScreen = true;
			} else if (value == "off") {
				isFullScreen = false;
			}
		}
		return isFullScreen;
	}
	bool ChooseWindowVisibility(const CmdLineArgs& cmdLineArgs) {
		// The argument can be one of the following:
		// --visible=on (default)
		// --visible=off
		bool isVisible{ true };
		if (cmdLineArgs.HasOption(cmdopt::visibleOpt)) {
			const Opt& opt = cmdLineArgs.GetOpt(cmdopt::visibleOpt);
			std::string_view value = opt.GetValue().GetString();
			if (value == "on") {
				isVisible = true;
			} else if (value == "off") {
				isVisible = false;
			}
		}
		return isVisible;
	}
	bool ChooseWindowResizeability(const CmdLineArgs& cmdLineArgs) {
		// The argument can be one of the following:
		// --resizable=on (default)
		// --resizable=off
		bool isResizable{ true };
		if (cmdLineArgs.HasOption(cmdopt::resizableOpt)) {
			const Opt& opt = cmdLineArgs.GetOpt(cmdopt::resizableOpt);
			std::string_view value = opt.GetValue().GetString();
			if (value == "on") {
				isResizable = true;
			} else if (value == "off") {
				isResizable = false;
			}
		}
		return isResizable;
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