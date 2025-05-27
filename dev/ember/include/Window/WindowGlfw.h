#pragma once

#include "Core/Util.h"
#include "Event/Event.h"
#include "GpuApi/GpuApiCtxOgl.h"
#include "Window/Window.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace ember {

	class WindowGlfw : public Window {
	public:
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
		// "Resizability" word is actually used!
		// Source: https://developer.apple.com/documentation/swiftui/windowresizability
		void SetResizability(int resize);
		void SetVisibility(int visibility);
		void SetClientApi(int api);

		void SetFramebufferDimensions();
		void RegisterGlfwCallbacks();

		void ProcessEvents();

		static void KeyboardKeyEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void WindowCloseEventCallback(GLFWwindow* window);
		static void FramebufferResizeEventCallback(GLFWwindow* window, int width, int height);
		static void GlfwErrorCallback(int errorCode, const char* description);

		GLFWwindow* windowHandle{ nullptr };
	};

}