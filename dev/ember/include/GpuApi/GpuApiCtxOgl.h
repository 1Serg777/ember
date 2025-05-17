#pragma once

#include "GpuApi/GpuApiCtx.h"
#include "Core/Util.h"

#include <string_view>

namespace ember {

	class Window;
	class WindowGlfw;
	class GpuApiCtxOgl;

	struct SettingsOgl {
		int openglVersionMajor{ 4 };
		int openglVersionMinor{ 6 };
		std::string_view glslVersion{ "#version 460" };
	};

	class WindowOglCtx {
	public:
		virtual void Initialize(const SettingsOgl& settings) = 0;
		virtual void Terminate() = 0;

		virtual void Present() = 0;

		virtual void MakeCurrent() = 0;
		virtual void MakeNonCurrent() = 0;
	};

	class GlfwOglCtx : public WindowOglCtx {
	public:
		GlfwOglCtx() = default;
		GlfwOglCtx(WindowGlfw* window, GpuApiCtxOgl* ctx);

		void Initialize(const SettingsOgl& settings) override;
		void Terminate() override;

		void Present() override;

		void MakeCurrent() override;
		void MakeNonCurrent() override;

	private:
		GpuApiCtxOgl* ctx{nullptr};
		WindowGlfw* window{nullptr};
	};

	class GpuApiCtxOgl : public GpuApiCtx {
	public:
		GpuApiCtxOgl(const SettingsOgl& settings);
		~GpuApiCtxOgl() = default;

		CLASS_NO_COPY(GpuApiCtxOgl);
		CLASS_NO_MOVE(GpuApiCtxOgl);

		void Initialize(Window* window) override;
		void Terminate() override;

		void Present() override;

		GpuApiType GetGpuApiType() const override;

		void MakeCurrent();
		void MakeNonCurrent();

		const SettingsOgl& GetSettingsOgl() const;

	private:
		SettingsOgl settings;
		WindowOglCtx* windowOglCtx{nullptr};
		Window* window{nullptr};
	};

	// GlfwGpuApiCtxOgl or GlfwOglGpuApiCtx
	// ... repeat for other Window APIs?
	// Problem is, we don't know about the Window API in use during context initialization.

	GpuApiCtx* CreateGpuApiCtxOgl();

	void SetCurrentGpuApiCtxOgl(GpuApiCtxOgl* gpuApiCtxOgl);
	GpuApiCtxOgl* GetCurrentGpuApiCtxOgl();

}