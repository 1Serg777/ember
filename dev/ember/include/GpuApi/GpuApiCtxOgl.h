#pragma once

#include "Core/Util.h"
#include "GpuApi/GpuApiCtx.h"

#include <string_view>
#include <memory>

namespace ember {

	class Window;
	class WindowGlfw;

	class GpuApiImGuiCtx;
	class OglGlfwImGuiCtx;

	struct SettingsOgl {
		int openglVersionMajor{4};
		int openglVersionMinor{6};
		std::string_view glslVersion{"#version 460"};
	};

	class GpuApiCtxOgl : public GpuApiCtx {
	public:
		GpuApiCtxOgl(const SettingsOgl& settings);
		virtual ~GpuApiCtxOgl() = default;

		CLASS_NO_COPY(GpuApiCtxOgl);
		CLASS_NO_MOVE(GpuApiCtxOgl);

		GpuApiType GetGpuApiType() const override;

		const SettingsOgl& GetSettingsOgl() const;

	protected:
		SettingsOgl settings;
	};

	class GlfwOglCtx : public GpuApiCtxOgl {
	public:
		GlfwOglCtx(const SettingsOgl& settings, WindowGlfw* window);
		CLASS_NO_COPY(GlfwOglCtx);
		CLASS_NO_MOVE(GlfwOglCtx);
		~GlfwOglCtx() = default;

		void Initialize() override;
		void InitializeGuiContext() override;
		void Terminate() override;
		void TerminateGuiContext() override;

		void OnMakeCurrent() override;
		void OnMakeNonCurrent() override;

		void OnFrameBegin() override;
		void OnFrameEnd() override;
		void DrawFrame() override;
		void Present() override;

		void OnFramebufferResize() override;

	private:
		WindowGlfw* window{nullptr};
		OglGlfwImGuiCtx* imGuiCtx{nullptr};
	};

	// GlfwGpuApiCtxOgl or GlfwOglGpuApiCtx
	// ... repeat for other Window APIs?
	// Problem is, we don't know about the Window API in use during context initialization.

	GpuApiCtxOgl* CreateGpuApiCtxOgl(Window* window);

	void SetCurrentGpuApiCtxOgl(GpuApiCtxOgl* gpuApiCtxOgl);
	GpuApiCtxOgl* GetCurrentGpuApiCtxOgl();

}