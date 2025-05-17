#include "GpuApi/GpuApiCtx.h"
#include "GpuApi/GpuApiCtxOgl.h"

#include <cassert>

namespace ember {

	static GpuApiCtx* currentGpuApiCtx{nullptr};

	GpuApiCtx* CreateGpuApiCtx(GpuApiType gpuApiType) {
		if (gpuApiType == GpuApiType::OPENGL) {
			return CreateGpuApiCtxOgl();
		} else if (gpuApiType == GpuApiType::VULKAN) {
			assert(false && "Vulkan is not supported yet!");
			return nullptr;
		} else {
			assert(false && "Invalid GPU API type id provided!");
			return nullptr;
		}
	}

	void SetCurrentGpuApiCtx(GpuApiCtx* gpuApiCtx) {
		if (gpuApiCtx->GetGpuApiType() == GpuApiType::OPENGL) {
			SetCurrentGpuApiCtxOgl(static_cast<GpuApiCtxOgl*>(gpuApiCtx));
		} else if (gpuApiCtx->GetGpuApiType() == GpuApiType::VULKAN) {
			assert(false && "Vulkan is not supported yet!");
		} else {
			assert(false && "Invalid GPU API type id provided!");
		}
		currentGpuApiCtx = gpuApiCtx;
	}

	GpuApiCtx* GetCurrentGpuApiCtx() {
		return currentGpuApiCtx;
	}

}