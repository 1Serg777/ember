#pragma once

#include "Event/EventRegistry.h"
#include "Math/MathUtil.h"

#include <string_view>
#include <cstdint>

namespace ember {

	enum class WindowApiType {
		EM_GLFW,
#ifdef EMBER_PLATFORM_WIN32
		EM_WIN32,
#elif EMBER_PLATFORM_LINUX
		EM_WAYLAND,
		EM_X11,
#endif
	};

	struct WindowSettings {
		std::string_view windowTitle{"Ember Level Editor"};

		Dimensions2D windowDimensions{1920, 1080};
		Dimensions2D framebufferDimensions{windowDimensions};

		WindowApiType type{WindowApiType::EM_GLFW};

		bool isFullscreen{false};
		bool isVisible{true};
		bool isResizable{true};

		bool setCallbacks{true};
	};

	class Window {
	public:
		Window(const WindowSettings& windowSettings);
		virtual ~Window();

		virtual void CreateWindow(void* dataPtr = nullptr) = 0;
		virtual void DestroyWindow() = 0;

		virtual void Update() = 0;

		virtual void SetEventRegistry(EventRegistry* eventRegistry);
		virtual void RemoveEventRegistry();

		const WindowSettings& GetWindowSettings() const;

		std::string_view GetWindowTitle() const;
		const Dimensions2D& GetWindowDimensions() const;

		WindowApiType GetWindowType() const;

		uint32_t GetWindowWidth() const;
		uint32_t GetWindowHeight() const;

		uint32_t GetFramebufferWidth() const;
		uint32_t GetFramebufferHeight() const;

		bool IsFullScreen() const;
		bool IsVisible() const;

	protected:
		WindowSettings windowSettings;
		EventRegistry* eventRegistry{nullptr};
	};

	Window* CreateWindow(const WindowSettings& windowSettings);

	void InitializeWindowLibrary(WindowApiType windowApiType);
	void TerminateWindowLibrary(WindowApiType windowApiType);

}