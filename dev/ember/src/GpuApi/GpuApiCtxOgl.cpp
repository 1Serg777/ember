#include "GpuApi/GpuApiCtxOgl.h"

#include "Window/Window.h"
#include "Window/WindowGlfw.h"

#include "Gui/ImGui/GpuApiImGuiCtx.h"

#include "imgui.h"
#include "Gui/ImGui/imgui_impl_glfw.h"
#include "Gui/ImGui/imgui_impl_opengl3.h"

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

	void GlfwOglCtx::Initialize() {
		window->InitializeOpenGLSpecific(settings);
		window->CreateGlfwWindow();
	}
	void GlfwOglCtx::InitializeGuiContext() {
		imGuiCtx = new OglGlfwImGuiCtx(this, window, settings.glslVersion.data());
		imGuiCtx->Initialize();
	}
	void GlfwOglCtx::Terminate() {
		window->DestroyWindow();
	}
	void GlfwOglCtx::TerminateGuiContext() {
		imGuiCtx->Terminate();
		delete imGuiCtx;
	}
	void GlfwOglCtx::OnFrameBegin() {
		imGuiCtx->OnFrameBegin();
	}
	void GlfwOglCtx::OnFrameEnd() {
		imGuiCtx->OnFrameEnd();
	}
	void GlfwOglCtx::DrawFrame() {
		glClearColor(215.0f / 255.0f, 153 / 255.0f, 33.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Our state
		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		ImGuiIO& io = ImGui::GetIO();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		imGuiCtx->RenderFrame();
	}
	void GlfwOglCtx::Present() {
		window->PresentFrame();
	}

	void GlfwOglCtx::OnFramebufferResize() {
		// TODO
	}

	void GlfwOglCtx::CreateMeshGpuResource(Mesh* mesh) {
		// TODO
	}
	void GlfwOglCtx::UploadMeshGpuResource(Mesh* mesh) {
		// TODO
	}

	void GlfwOglCtx::OnMakeCurrent() {
		window->MakeContextCurrent();
		if (!openglFunctionsLoaded) {
			WindowGlfw::LoadOpenGlFunctions();
		}
	}
	void GlfwOglCtx::OnMakeNonCurrent() {
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
		else if (windowType == WindowApiType::EM_XLIB) {
			assert(false && "[OGL Context] XLIB Window API is not supported yet!");
			return nullptr;
		} else if (windowType == WindowApiType::EM_XCB) {
			assert(false && "[OGL Context] XCB Window API is not supported yet!");
			return nullptr;
		} else if (windowType == WindowApiType::EM_WAYLAND) {
			assert(false && "[OGL Context] Wayland Window API is not supported yet!");
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
			currentGpuApiCtxOgl->OnMakeNonCurrent();
		}
		gpuApiCtxOgl->OnMakeCurrent();
		currentGpuApiCtxOgl = gpuApiCtxOgl;
	}
	GpuApiCtxOgl* GetCurrentGpuApiCtxOgl() {
		return currentGpuApiCtxOgl;
	}

}