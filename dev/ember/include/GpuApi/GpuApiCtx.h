#pragma once

namespace ember {

	class Window;

	enum class GpuApiType {
		NONE = 0,
		OPENGL = 1,
		VULKAN = 2,
	};

	class GpuApiCtx {
	public:
		virtual ~GpuApiCtx() = default;

		virtual void Initialize(Window* window) = 0;
		virtual void Terminate() = 0;

		virtual void Present() = 0;

		virtual GpuApiType GetGpuApiType() const = 0;
	};

	/* Add more configuration parameters? */
	GpuApiCtx* CreateGpuApiCtx(GpuApiType gpuApiType);

	void SetCurrentGpuApiCtx(GpuApiCtx* gpuApiCtx);
	GpuApiCtx* GetCurrentGpuApiCtx();

}