#include "EmberLvlEditorApp.h"

#include "Core/CmdLineArgs.h"
#include "Core/Error.h"

#include "GpuApi/GpuApiCtx.h"

#include "Window/Window.h"

#include <cstdlib>
#include <iostream>
#include <functional>
#include <memory>

namespace ember {

	EmberLvlEditorApp::EmberLvlEditorApp(const CmdLineArgs& cmdLineArgs) {
	}

	void EmberLvlEditorApp::Initialize() {
		InitializeLibraries();
		InitializeSystems();
		RegisterApplicationCallbacks();
	}
	void EmberLvlEditorApp::Terminate() {
		TerminateSystems();
		TerminateLibraries();
	}

	int EmberLvlEditorApp::Run() {
		appIsRunning = true;
		try {
			while (appIsRunning) {
				gpuApiCtx->Present();
				window->Update();
				eventRegistry->Update();
			}
		} catch (const GLFWError& glfwError) {
			std::cerr << "[GLFW Error]: " << glfwError.what() << std::endl;
			return EXIT_FAILURE;
		} catch (const EmberError& emberError) {
			std::cerr << "[Ember Error]: " << emberError.what() << std::endl;
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	void EmberLvlEditorApp::InitializeLibraries() {
		InitializeWindowLibrary(WindowApiType::EM_GLFW);
	}
	void EmberLvlEditorApp::InitializeSystems() {
		eventRegistry = std::make_unique<EventRegistry>();

		// Later, these settings will probably be retrieved from some configuration file.
		GpuApiType gpuApiType = GpuApiType::OPENGL;
		WindowSettings windowSettings{};
		windowSettings.type = WindowApiType::EM_GLFW;

		// Here the objects (of the apporpriate classes according to the settings)
		// are created and their settings are set up.
		// However, the actual initialization happens later.
		window = std::unique_ptr<Window>(CreateWindow(windowSettings));
		gpuApiCtx = std::unique_ptr<GpuApiCtx>(CreateGpuApiCtx(gpuApiType, window.get()));

		InitializeWindowAndGpuApiContext(window.get(), gpuApiCtx.get());
		SetCurrentGpuApiCtx(gpuApiCtx.get());
	}
	void EmberLvlEditorApp::TerminateLibraries() {
		TerminateWindowLibrary(WindowApiType::EM_GLFW);
	}
	void EmberLvlEditorApp::TerminateSystems() {
		GpuApiType gpuApiType = gpuApiCtx->GetGpuApiType();
		gpuApiCtx->Terminate();
		// If OpenGL was used, then window destruction has already happened.
		// This is because in order to terminate an OpenGL context the corresponding
		// window, that was created during context initialization, must be destroyed.
		// in the Terminate() call of the GPU API context.
		if (gpuApiType != GpuApiType::OPENGL) {
			window->DestroyWindow();
		}
	}

	void EmberLvlEditorApp::InitializeWindowAndGpuApiContext(Window* window, GpuApiCtx* gpuApiCtx) {
		window->SetEventRegistry(eventRegistry.get());
		// OpenGL is inherently tied to windows. In other words,
		// when a window is created, a new OpenGL context is created as well.
		// Vulkan, on the other hand, makes no assumptions about windows, so
		// it can be created separately and supplied later during initialization (for surface creation).
		// If OpenGL is used, then window creation happens during context initialization.
		if (gpuApiCtx->GetGpuApiType() != GpuApiType::OPENGL) {
			window->CreateWindow();
		}
		gpuApiCtx->Initialize(window);
	}

	void EmberLvlEditorApp::RegisterApplicationCallbacks() {
		using namespace std::placeholders;

		eventRegistry->RegisterEventCallback<KeyboardKeyEventData>(
			std::bind(&EmberLvlEditorApp::OnKeyboardKeyEvent, this, _1));
		eventRegistry->RegisterEventCallback<WindowCloseEventData>(
			std::bind(&EmberLvlEditorApp::OnWindowClose, this, _1));
	}
	void EmberLvlEditorApp::OnKeyboardKeyEvent(const KeyboardKeyEventData& keyboardKeyEventData) {
		switch (keyboardKeyEventData.key) {
			case Keyboard::KeyCode::EM_KEY_ESC:
				this->appIsRunning = false;
				break;
		}
	}
	void EmberLvlEditorApp::OnWindowClose(const WindowCloseEventData& windowCloseEventData) {
		if (windowCloseEventData.close) {
			this->appIsRunning = false;
		}
	}
}