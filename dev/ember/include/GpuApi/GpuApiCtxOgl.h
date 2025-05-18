#pragma once

#include "Core/Util.h"
#include "GpuApi/GpuApiCtx.h"
#include "Window/Window.h"

#include <string_view>

namespace ember {

	class WindowGlfw;

	struct SettingsOgl {
		int openglVersionMajor{ 4 };
		int openglVersionMinor{ 6 };
		std::string_view glslVersion{ "#version 460" };
	};

	class GpuApiCtxOgl : public GpuApiCtx {
	public:
		GpuApiCtxOgl(const SettingsOgl& settings);
		virtual ~GpuApiCtxOgl() = default;

		CLASS_NO_COPY(GpuApiCtxOgl);
		CLASS_NO_MOVE(GpuApiCtxOgl);

		GpuApiType GetGpuApiType() const override;

		virtual void Initialize(Window* window = nullptr) = 0;

		virtual void MakeCurrent() = 0;
		virtual void MakeNonCurrent() = 0;

		const SettingsOgl& GetSettingsOgl() const;

	protected:
		SettingsOgl settings;
	};

	class GlfwOglCtx : public GpuApiCtxOgl {
	public:
		GlfwOglCtx(const SettingsOgl& settings, WindowGlfw* window);
		~GlfwOglCtx() = default;

		void Initialize(Window* window) override;
		void Terminate() override;
		void Present() override;

		void MakeCurrent() override;
		void MakeNonCurrent() override;

	private:
		WindowGlfw* window{ nullptr };
	};

	// GlfwGpuApiCtxOgl or GlfwOglGpuApiCtx
	// ... repeat for other Window APIs?
	// Problem is, we don't know about the Window API in use during context initialization.

	GpuApiCtxOgl* CreateGpuApiCtxOgl(Window* window);

	void SetCurrentGpuApiCtxOgl(GpuApiCtxOgl* gpuApiCtxOgl);
	GpuApiCtxOgl* GetCurrentGpuApiCtxOgl();

}