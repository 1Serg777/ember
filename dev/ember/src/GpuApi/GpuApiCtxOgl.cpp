#include "GpuApi/GpuApiCtxOgl.h"

#include "Window/Window.h"
#include "Window/WindowGlfw.h"

#include <cassert>
#include <stdexcept>

namespace ember {

	static GpuApiCtxOgl* currentGpuApiCtxOgl{nullptr};
	static bool openglFunctionsLoaded{false};

	GpuApiCtxOgl::GpuApiCtxOgl(const SettingsOgl& settings)
		: settings(settings) {
	}
	GpuApiType GpuApiCtxOgl::GetGpuApiType() const {
		return GpuApiType::OPENGL;
	}
	const SettingsOgl& GpuApiCtxOgl::GetSettingsOgl() const {
		return settings;
	}

	GlfwOglCtx::GlfwOglCtx(const SettingsOgl& settings, WindowGlfw* window)
		: GpuApiCtxOgl(settings), window(window) {
	}

	void GlfwOglCtx::Initialize(Window* window) {
		// assert(this->window == window && "[GLFW OGL Context] Initializing OGL context with a wrong window!");
		this->window->InitializeOpenGLSpecific(settings);
		this->window->CreateGlfwWindow();
	}
	void GlfwOglCtx::Terminate() {
		window->DestroyWindow();
	}
	void GlfwOglCtx::Present() {
		glClearColor(215.0f / 255.0f, 153 / 255.0f, 33.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		window->PresentFrame();
	}

	void GlfwOglCtx::MakeCurrent() {
		window->MakeContextCurrent();
		if (!openglFunctionsLoaded) {
			WindowGlfw::LoadOpenGlFunctions();
		}
	}
	void GlfwOglCtx::MakeNonCurrent() {
		window->MakeContextNonCurrent();
	}

	GpuApiCtxOgl* CreateGpuApiCtxOgl(Window* window) {
		SettingsOgl defaultSettings{};
		WindowApiType windowType = window->GetWindowType();
		if (windowType == WindowApiType::EM_GLFW) {
			GlfwOglCtx* glfwOglCtx = new GlfwOglCtx(defaultSettings, static_cast<WindowGlfw*>(window));
			return glfwOglCtx; // The ownership is transferred to the caller!
		}
#ifdef EMBER_PLATFORM_WIN32
		else if (windowType == WindowApiType::EM_WIN32) {
			assert(false && "[OGL Context] Win32 Window API is not supported yet!");
			return nullptr;
		}
#elif EMBER_PLATFORM_LINUX
		else if (windowType == WindowApiType::WAYLAND) {
			assert(false && "[OGL Context] Wayland Window API is not supported yet!");
			return nullptr;
		}
		else if (windowType == WindowApiType::X11) {
			assert(false && "[OGL Context] X11 Window API is not supported yet!");
			return nullptr;
		}
#endif
		else {
			assert(false && "[Window] Unsupported Window API!");
			return nullptr;
		}
	}

	void SetCurrentGpuApiCtxOgl(GpuApiCtxOgl* gpuApiCtxOgl) {
		if (currentGpuApiCtxOgl) {
			currentGpuApiCtxOgl->MakeNonCurrent();
		}
		gpuApiCtxOgl->MakeCurrent();
		currentGpuApiCtxOgl = gpuApiCtxOgl;
	}
	GpuApiCtxOgl* GetCurrentGpuApiCtxOgl() {
		return currentGpuApiCtxOgl;
	}

}