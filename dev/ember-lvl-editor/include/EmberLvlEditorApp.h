#pragma once

#include "Core/CmdLineArgs.h"
#include "GpuApi/GpuApiCtx.h"
#include "Window/Window.h"

#include <memory>

namespace ember {

    class EmberLvlEditorApp {
    public:
        EmberLvlEditorApp(const CmdLineArgs& cmdLineArgs);

        void Initialize();
        void Terminate();

        int Run();

    private:
        void InitializeLibraries();
        void InitializeSystems();
        void TerminateLibraries();
        void TerminateSystems();

        void InitializeWindowAndGpuApiContext(Window* window, GpuApiCtx* gpuApiCtx);

        std::unique_ptr<GpuApiCtx> gpuApiCtx;
        std::unique_ptr<Window> window;
    };

}