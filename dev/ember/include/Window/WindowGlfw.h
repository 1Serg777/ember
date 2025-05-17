#pragma once

#include "Window/Window.h"
#include "Core/Util.h"
#include "GpuApi/GpuApiCtxOgl.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace ember {

	class WindowGlfw : public Window {
	public:
		static void GlfwErrorCallback(int errorCode, const char* description);

		static void InitializeGlfwLibrary();
		static void TerminateGlfwLibrary();
		static bool GlfwLibraryInitialized();
		static void LoadOpenGlFunctions();

		WindowGlfw(const WindowSettings& windowSettings);
		virtual ~WindowGlfw() = default;

		CLASS_NO_COPY(WindowGlfw);
		CLASS_DEFAULT_MOVE(WindowGlfw);

		void CreateWindow(void* dataPtr) override;
		void DestroyWindow() override;

		void Update() override;

		void CreateGlfwWindow(WindowGlfw* sharedCtxWindow = nullptr);

		// Rendering API specific methods
		// -----------------------------------------------------------------
		// -----------------------------------------------------------------
		
		// 1. OpenGL

		void InitializeOpenGLSpecific(const SettingsOgl& oglSettings);
		void MakeContextCurrent();
		void MakeContextNonCurrent();

		void PresentFrame();

		// 2. Vulkan

		void InitializeVulkanSpecific(/* const VulkanSettings& vulkanSettings */);

		// -----------------------------------------------------------------
		// -----------------------------------------------------------------

		GLFWwindow* GetApiSpecificHandle() const;

	private:
		void InitializeWindowGlfwParams();
		void SetVisibility(int visibility);
		void SetClientApi(int api);
		void RegisterGlfwCallbacks();

		void ProcessEvents();

		GLFWwindow* windowHandle{ nullptr };
	};

}