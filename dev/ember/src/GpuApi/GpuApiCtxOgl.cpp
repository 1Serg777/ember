#include "GpuApi/GpuApiCtxOgl.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cassert>
#include <stdexcept>

namespace ember {

	static GpuApiCtxOgl* gpuApiCtxOgl{ nullptr };

	GpuApiType GpuApiCtxOgl::GetGpuApiType() const {
		return GpuApiType::OPENGL;
	}

	const SettingsOgl& GpuApiCtxOgl::GetSettingsOgl() const {
		return settings;
	}
	const SettingsGlfw& GpuApiCtxOgl::GetSettingsGlfw() const {
		return settings.glfwSettings;
	}

	void InitializeGpuApiCtxOgl() {
		gpuApiCtxOgl = new GpuApiCtxOgl();
	}
	void TerminateGpuApiCtxOgl() {
		delete gpuApiCtxOgl;
	}

	GpuApiCtxOgl* GetGpuApiCtxOgl() {
		return gpuApiCtxOgl;
	}

}