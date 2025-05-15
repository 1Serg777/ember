#pragma once

#include "GpuApi/GpuApiCtx.h"
#include "Core/Util.h"

#include <string_view>

namespace ember {

	struct SettingsGlfw {
		int openglVersionMajor{4};
		int openglVersionMinor{6};

		std::string_view glslVersion{"#version 460"};
	};

	struct SettingsOgl {
		SettingsGlfw glfwSettings;
	};

	class GpuApiCtxOgl : public GpuApiCtx {
	public:
		GpuApiCtxOgl() = default;
		~GpuApiCtxOgl() = default;

		CLASS_NO_COPY(GpuApiCtxOgl);
		CLASS_NO_MOVE(GpuApiCtxOgl);

		GpuApiType GetGpuApiType() const override;

		const SettingsOgl& GetSettingsOgl() const;
		const SettingsGlfw& GetSettingsGlfw() const;

	private:
		SettingsOgl settings{};
	};

	void InitializeGpuApiCtxOgl();
	void TerminateGpuApiCtxOgl();

	GpuApiCtxOgl* GetGpuApiCtxOgl();

}