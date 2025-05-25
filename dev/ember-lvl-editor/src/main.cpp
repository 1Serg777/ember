#include "Core/CmdLineArgs.h"
#include "EmberLvlEditorApp.h"

#include <cstdlib>
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) {
    ember::CmdLineArgs cmdLineArgs{ argc, argv };
    ember::EmberLvlEditorApp app{ cmdLineArgs };
    if (!app.Initialize()) {
        return EXIT_FAILURE;
    }
    int exitCode = app.Run();
    app.Terminate();
    return exitCode;
}