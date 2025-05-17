#include "GpuApi/GpuApiCtxOgl.h"

#include "Window/Window.h"
#include "Window/WindowGlfw.h"

#include <cassert>
#include <stdexcept>

namespace ember {

	static GpuApiCtxOgl* currentGpuApiCtxOgl{nullptr};
	static bool openglFunctionsLoaded{false};

	GlfwOglCtx::GlfwOglCtx(WindowGlfw* window, GpuApiCtxOgl* ctx)
		: ctx(ctx), window(window) {
	}

	void GlfwOglCtx::Initialize(const SettingsOgl& settings) {
		window->InitializeOpenGLSpecific(settings);
		window->CreateGlfwWindow();
	}
	void GlfwOglCtx::Terminate() {
		window->DestroyWindow();
	}

	void GlfwOglCtx::Present() {
		window->PresentFrame();
	}

	void GlfwOglCtx::MakeCurrent() {
		window->MakeContextCurrent();
	}
	void GlfwOglCtx::MakeNonCurrent() {
		window->MakeContextNonCurrent();
	}

	GpuApiCtxOgl::GpuApiCtxOgl(const SettingsOgl& settings)
		: settings(settings) {
	}

	void GpuApiCtxOgl::Initialize(Window* window) {
		if (window->GetWindowType() == WindowApiType::EM_GLFW) {
			WindowGlfw* windowGlfw = static_cast<WindowGlfw*>(window);
			windowGlfw->InitializeOpenGLSpecific(settings);
			windowGlfw->CreateGlfwWindow();
		} else if (window->GetWindowType() == WindowApiType::EM_WIN32) {
			// TODO: implement Win32 API Window
			assert(false && "Win32 Window API is not supported with OpenGL yet!");
		} else {
			assert(false && "Unsupported Window API!");
		}
		this->window = window;
	}
	void GpuApiCtxOgl::Terminate() {
		if (window->GetWindowType() == WindowApiType::EM_GLFW) {
			WindowGlfw* windowGlfw = static_cast<WindowGlfw*>(window);
			windowGlfw->DestroyWindow();
		} else if (window->GetWindowType() == WindowApiType::EM_WIN32) {
			// TODO: implement Win32 API Window
			assert(false && "Win32 Window API is not supported with OpenGL yet!");
		} else {
			assert(false && "Unsupported Window API!");
		}
		window = nullptr;
	}

	void GpuApiCtxOgl::Present() {
		if (window->GetWindowType() == WindowApiType::EM_GLFW) {
			WindowGlfw* windowGlfw = static_cast<WindowGlfw*>(window);
			windowGlfw->PresentFrame();
		}
		else if (window->GetWindowType() == WindowApiType::EM_WIN32) {
			// TODO: implement Win32 API Window
			assert(false && "Win32 Window API is not supported with OpenGL yet!");
		}
		else {
			assert(false && "Unsupported Window API!");
		}
	}

	GpuApiType GpuApiCtxOgl::GetGpuApiType() const {
		return GpuApiType::OPENGL;
	}

	void GpuApiCtxOgl::MakeCurrent() {
		assert(window && "The context wasn't properly initialized! Window pointer was 'nullptr'!");
		if (window->GetWindowType() == WindowApiType::EM_GLFW) {
			WindowGlfw* windowGlfw = static_cast<WindowGlfw*>(window);
			windowGlfw->MakeContextCurrent();
			if (!openglFunctionsLoaded) {
				WindowGlfw::LoadOpenGlFunctions();
			}
		} else if (window->GetWindowType() == WindowApiType::EM_WIN32) {
			// TODO: implement Win32 API Window
			assert(false && "Win32 Window API is not supported with OpenGL yet!");
		} else {
			assert(false && "Unsupported Window API!");
		}
	}
	void GpuApiCtxOgl::MakeNonCurrent() {
		assert(window && "The context wasn't properly initialized! Window pointer was 'nullptr'!");
		if (window->GetWindowType() == WindowApiType::EM_GLFW) {
			WindowGlfw* windowGlfw = static_cast<WindowGlfw*>(window);
			windowGlfw->MakeContextNonCurrent();
		}
		else if (window->GetWindowType() == WindowApiType::EM_WIN32) {
			// TODO: implement Win32 API Window
			assert(false && "Win32 Window API is not supported with OpenGL yet!");
		}
		else {
			assert(false && "Unsupported Window API!");
		}
	}

	const SettingsOgl& GpuApiCtxOgl::GetSettingsOgl() const {
		return settings;
	}

	GpuApiCtx* CreateGpuApiCtxOgl() {
		SettingsOgl defaultSettings{};
		GpuApiCtxOgl* gpuApiCtxOgl = new GpuApiCtxOgl(defaultSettings);
		return gpuApiCtxOgl; // The ownership is transferred to the caller!
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