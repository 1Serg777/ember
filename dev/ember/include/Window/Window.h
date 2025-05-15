#pragma once

#include "GpuApi/GpuApiCtx.h"
#include "Math/MathUtil.h"

#include <string_view>
#include <cstdint>

namespace ember {

	struct WindowSettings {
		std::string_view windowTitle{"Ember"};

		Dimensions2D windowDimensions{1920, 1080};
		Dimensions2D framebufferDimensions{windowDimensions};

		bool isFullscreen{false};
		bool isVisible{true};
	};

	class Window {
	public:
		Window(const WindowSettings& windowSettings);
		virtual ~Window();

		virtual void Update() = 0;

		virtual void SetEventRegistry(/* EventRegistry* eventRegistry */);
		virtual void RemoveEventRegistry();

		const WindowSettings& GetWindowSettings() const;

		std::string_view GetWindowTitle() const;
		const Dimensions2D& GetWindowDimensions() const;

		uint32_t GetWindowWidth() const;
		uint32_t GetWindowHeight() const;

		bool IsFullScreen() const;
		bool IsVisible() const;

	protected:
		WindowSettings windowSettings;
	};

	Window* CreateWindow(const WindowSettings& windowSettings, GpuApiType gpuApiType);

}