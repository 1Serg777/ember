#pragma once

#include "Core/CmdLineArgs.h"
#include "GpuApi/GpuApiCtx.h"
#include "Event/EventRegistry.h"
#include "Window/Window.h"

#include <memory>

namespace ember {

    class EmberLvlEditorApp {
    public:
        EmberLvlEditorApp(const CmdLineArgs& cmdLineArgs);

        bool Initialize();
        void Terminate();

        int Run();

    private:
        void InitializeLibraries();
        void InitializeSystems();

        void TerminateLibraries();
        void TerminateSystems();

        void InitializeWindowAndGpuApiContext(Window* window, GpuApiCtx* gpuApiCtx);
        void InitializeGuiContext();

        void RegisterApplicationCallbacks();
        void OnKeyboardKeyEvent(const KeyboardKeyEventData& keyboardKeyEventData);
        void OnWindowClose(const WindowCloseEventData& windowCloseEventData);
        void OnFramebufferResizeEvent(const FramebufferResizeEventData& framebufferResizeEventData);

        CmdLineArgs cmdLineArgs;
        std::unique_ptr<EventRegistry> eventRegistry;
        std::unique_ptr<GpuApiCtx> gpuApiCtx;
        std::unique_ptr<Window> window;

        bool appIsRunning{false};
    };

}