#pragma once

#include "Core/CmdLineArgs.h"
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
        void InitializeSystems();
        void InitializeWindowAndGpuApiContext();

        std::unique_ptr<Window> window;
    };

}