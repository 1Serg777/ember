#include "EmberLvlEditorApp.h"

#include "Core/CmdLineArgs.h"
#include "Core/Error.h"

#include "GpuApi/GpuApiCtx.h"

#include "Window/Window.h"

#include <cstdlib>
#include <iostream>
#include <memory>

namespace ember {

	EmberLvlEditorApp::EmberLvlEditorApp(const CmdLineArgs& cmdLineArgs) {
	}

	void EmberLvlEditorApp::Initialize() {
		InitializeLibraries();
		InitializeSystems();
	}
	void EmberLvlEditorApp::Terminate() {
		TerminateSystems();
		TerminateLibraries();
	}

	int EmberLvlEditorApp::Run() {
		try {
			while (true) {
				window->Update();
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
		// The settings later will probably be retrieved from some configuration file.
		GpuApiType gpuApiType = GpuApiType::OPENGL;
		WindowSettings windowSettings{};

		// Here the objects (of the apporpriate classes according to the settings)
		// are created and their settings are set up. The initialization happens later.
		window = std::unique_ptr<Window>(CreateWindow(windowSettings));
		gpuApiCtx = std::unique_ptr<GpuApiCtx>(CreateGpuApiCtx(gpuApiType));

		InitializeWindowAndGpuApiContext(window.get(), gpuApiCtx.get());
		SetCurrentGpuApiCtx(gpuApiCtx.get());
	}
	void EmberLvlEditorApp::TerminateLibraries() {
		TerminateWindowLibrary(WindowApiType::EM_GLFW);
	}
	void EmberLvlEditorApp::TerminateSystems() {
		GpuApiType gpuApiType = gpuApiCtx->GetGpuApiType();
		gpuApiCtx->Terminate();
		// If the API was OpenGL, then window->DestroyWindow() has already been called
		// in the Terminate() call of the GPU API context.
		if (gpuApiType != GpuApiType::OPENGL) {
			window->DestroyWindow();
		}
	}

	void EmberLvlEditorApp::InitializeWindowAndGpuApiContext(Window* window, GpuApiCtx* gpuApiCtx) {
		// 1. Vulkan isn't inherently tied to windows,
		//    so a window can be created separately from Vulkan context.
		if (gpuApiCtx->GetGpuApiType() != GpuApiType::OPENGL) {
			window->CreateWindow();
		}
		// 2. An OpenGL context, on the other hand, is created when a window is created.
		//    Additionally, we must specify certain parameters (GLFW calls them hints),
		//    before a window (and therefore a context) is created.
		//    OpenGL's Context class will take care of both window and context creation.
		gpuApiCtx->Initialize(window);
	}
}