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
		// SetAppSettings();

		// initialize window
		// initialize gpu api context
		// initialize renderer
		// 
		// load the critical assets first
		// load the recent project if any
		//     - load the project's assets
		// ...
		InitializeSystems();
	}
	void EmberLvlEditorApp::Terminate() {
	}

	int EmberLvlEditorApp::Run() {
		try {
			// TODO: app logic
		} catch (const GLFWError& glfwError) {
			std::cerr << "[GLFW Error]: " << glfwError.what() << std::endl;
			return EXIT_FAILURE;
		} catch (const EmberError& emberError) {
			std::cerr << "[Ember Error]: " << emberError.what() << std::endl;
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;
	}

	void EmberLvlEditorApp::InitializeSystems() {
		InitializeWindowAndGpuApiContext();
	}
	void EmberLvlEditorApp::InitializeWindowAndGpuApiContext() {
		// Because of OpenGL and how its context is tied to the window,
		// we handle both operations as a single unit.
		// With Vulkan, it's fine to initialize the window first,
		// and then the Vulkan library itself.

		WindowSettings defaultWindowSettings{};
		GpuApiType gpuApiType = GpuApiType::OPENGL; // The only one supported for now.
		if (gpuApiType == GpuApiType::OPENGL) {
			InitializeGpuApiCtx(gpuApiType);
			window = std::unique_ptr<Window>(CreateWindow(defaultWindowSettings, gpuApiType));
		} else if (gpuApiType == GpuApiType::VULKAN) {
			assert(false && "Vulkan is not supported yet!");
		} else {
			assert(false && "Invalid GPU API type id provided!");
		}
	}
}