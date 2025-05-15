#pragma once

namespace ember {

	enum class GpuApiType {
		NONE = 0,
		OPENGL = 1,
		VULKAN = 2,
	};

	class GpuApiCtx {
	public:
		virtual GpuApiType GetGpuApiType() const = 0;
	};

	void InitializeGpuApiCtx(GpuApiType gpuApiType);
	void TerminateGpuApiCtx();
	
	GpuApiCtx* GetGpuApiCtx();

}