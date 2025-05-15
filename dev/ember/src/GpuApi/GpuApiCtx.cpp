#include "GpuApi/GpuApiCtx.h"
#include "GpuApi/GpuApiCtxOgl.h"

#include <cassert>

namespace ember {

	static GpuApiCtx* gpuApiCtx{nullptr};

	void InitializeGpuApiCtx(GpuApiType gpuApiType) {
		// 1. GPU API Context initialization when the project is already running.
		//    Possibly the user requested to switch to a different GPU API.
		if (gpuApiCtx) {
			if (gpuApiCtx->GetGpuApiType() == gpuApiType)
				return;
			TerminateGpuApiCtx();
		}

		// 2. Startup initialization
		if (gpuApiType == GpuApiType::OPENGL) {
			InitializeGpuApiCtxOgl();
			gpuApiCtx = GetGpuApiCtxOgl();
		} else if (gpuApiType == GpuApiType::VULKAN) {
			assert(false && "Vulkan is not supported yet!");
		} else {
			assert(false && "Invalid GPU API type id provided!");
		}
	}
	void TerminateGpuApiCtx() {
		assert(gpuApiCtx != nullptr && "Gpu Api Context must be created!");
		if (gpuApiCtx->GetGpuApiType() == GpuApiType::OPENGL) {
			TerminateGpuApiCtxOgl();
		} else if (gpuApiCtx->GetGpuApiType() == GpuApiType::VULKAN) {
			assert(false && "Vulkan is not supported yet!");
		} else {
			assert(false && "Invalid GPU API type id provided!");
		}
		gpuApiCtx = nullptr;
	}

	GpuApiCtx* GetGpuApiCtx() {
		return gpuApiCtx;
	}

}