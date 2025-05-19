#pragma once

namespace ember {

	class WindowGlfw;
	class GlfwOglCtx;

	class GpuApiImGuiCtx {
	public:
		virtual ~GpuApiImGuiCtx() = default;

		virtual void Initialize() = 0;
		virtual void Terminate() = 0;

		virtual void OnFrameBegin() = 0;
		virtual void RenderFrame() = 0;
		virtual void OnFrameEnd() = 0;
	};

	class OglGlfwImGuiCtx : public GpuApiImGuiCtx {
	public:
		OglGlfwImGuiCtx(GlfwOglCtx* ctx, WindowGlfw* window, const char* glslVersion);
		~OglGlfwImGuiCtx() = default;

		void Initialize() override;
		void Terminate() override;

		void OnFrameBegin() override;
		void RenderFrame() override;
		void OnFrameEnd() override;

	private:
		GlfwOglCtx* ctx{nullptr};
		WindowGlfw* window{nullptr};
		const char* glslVersion{nullptr};
	};

}