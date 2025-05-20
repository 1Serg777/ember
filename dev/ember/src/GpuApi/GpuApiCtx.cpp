#include "GpuApi/GpuApiCtx.h"
#include "GpuApi/GpuApiCtxOgl.h"
#include "GpuApi/GpuApiCtxVk.h"

#include <cassert>

namespace ember {

	static GpuApiCtx* currentGpuApiCtx{nullptr};

	GpuApiCtx* CreateGpuApiCtx(GpuApiType gpuApiType, Window* window) {
		if (gpuApiType == GpuApiType::OPENGL) {
			return CreateGpuApiCtxOgl(window);
		} else if (gpuApiType == GpuApiType::VULKAN) {
			return CreateGpuApiCtxVk(window);
		} else {
			assert(false && "Invalid GPU API type id provided!");
			return nullptr;
		}
	}
	void SetCurrentGpuApiCtx(GpuApiCtx* gpuApiCtx) {
		if (gpuApiCtx->GetGpuApiType() == GpuApiType::OPENGL) {
			SetCurrentGpuApiCtxOgl(static_cast<GpuApiCtxOgl*>(gpuApiCtx));
		} else if (gpuApiCtx->GetGpuApiType() == GpuApiType::VULKAN) {
			SetCurrentGpuApiCtxVk(static_cast<GpuApiCtxVk*>(gpuApiCtx));
		} else {
			assert(false && "Invalid GPU API type id provided!");
		}
		currentGpuApiCtx = gpuApiCtx;
	}
	GpuApiCtx* GetCurrentGpuApiCtx() {
		return currentGpuApiCtx;
	}

}