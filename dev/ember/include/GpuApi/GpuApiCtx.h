#pragma once

#include "Window/Window.h"

namespace ember {

	enum class GpuApiType {
		NONE = 0,
		OPENGL = 1,
		VULKAN = 2,
	};

	class GpuApiCtx {
	public:
		virtual ~GpuApiCtx() = default;

		virtual void Initialize(Window* window) = 0;
		virtual void InitializeGuiContext(Window* window) = 0;
		virtual void Terminate() = 0;
		virtual void TerminateGuiContext() = 0;

		virtual void OnFrameBegin() = 0;
		virtual void OnFrameEnd() = 0;
		virtual void DrawFrame() = 0;
		virtual void Present() = 0;

		virtual GpuApiType GetGpuApiType() const = 0;
	};

	/* Add more configuration parameters? */
	GpuApiCtx* CreateGpuApiCtx(GpuApiType gpuApiType, Window* window);

	void SetCurrentGpuApiCtx(GpuApiCtx* gpuApiCtx);
	GpuApiCtx* GetCurrentGpuApiCtx();

}