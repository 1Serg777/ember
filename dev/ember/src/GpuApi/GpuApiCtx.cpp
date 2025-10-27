#include "GpuApi/GpuApiCtx.h"
#include "GpuApi/GpuApiCtxOgl.h"
#include "GpuApi/GpuApiCtxVk.h"

#include <cassert>

namespace ember {

	static GpuApiCtx* currentGpuApiCtx{nullptr};

	GpuApiType ChooseGpuApi(const CmdLineArgs& cmdLineArgs) {
		GpuApiType gpuApiType = GpuApiType::OPENGL;
		if (cmdLineArgs.HasOption(cmdopt::gpuApiOpt)) {
			// --gpuapi[=apiname], where
			//   apiname is one of
			//   1) opengl
			//   2) vulkan
			const Opt& opt = cmdLineArgs.GetOpt(cmdopt::gpuApiOpt);
			std::string_view value = opt.GetValue().GetString();
			if (value == cmdopt::gpuApiOpenglVal) {
				gpuApiType = GpuApiType::OPENGL;
			} else if (value == cmdopt::gpuApiVulkanVal) {
				gpuApiType = GpuApiType::VULKAN;
			}
			// At this point we're 100% sure that the options passed are correct.
			// In other words, the check I was trying to do below has already been done in the parser.
			/*
			else {
				std::cerr << "Unknown GPU API type passed in the command line arguments: "
			}
			*/
		}
		return gpuApiType;
	}

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

	void CreateMeshGpuResource(Mesh* mesh) {
		currentGpuApiCtx->CreateMeshGpuResource(mesh);
	}
	void UploadMeshGpuResource(Mesh* mesh) {
		currentGpuApiCtx->UploadMeshGpuResource(mesh);
	}

}