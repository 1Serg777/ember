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

		static void LoadOpenGlFunctions();

		WindowGlfw(const WindowSettings& windowSettings);
		virtual ~WindowGlfw() = default;

		CLASS_NO_COPY(WindowGlfw);
		CLASS_DEFAULT_MOVE(WindowGlfw);

		void Update() override;

		// Rendering API specific methods
		// -----------------------------------------------------------------
		// -----------------------------------------------------------------
		
		// 1. OpenGL

		void InitializeOpenGLSpecific(const SettingsGlfw& glfwOglSettings);
		void MakeContextCurrent();
		void SetSwapInterval(int swapInterval);

		// void PresentFrame();

		// 2. Vulkan

		void InitializeVulkanSpecific(/* const VulkanSettings& vulkanSettings */);

		// -----------------------------------------------------------------
		// -----------------------------------------------------------------

		void CreateWindow(WindowGlfw* sharedCtxWindow = nullptr); // OpenGL shared context window if any.
		void DestroyWindow();

		GLFWwindow* GetApiSpecificHandle() const;

	private:
		void InitializeWindowGlfwParams();
		void CreateGlfwWindow(WindowGlfw* sharedCtxWindow);
		void RegisterGlfwCallbacks();

		GLFWwindow* windowHandle{ nullptr };
	};

}