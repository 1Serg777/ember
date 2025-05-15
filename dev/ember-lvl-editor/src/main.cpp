#include "Core/CmdLineArgs.h"
#include "EmberLvlEditorApp.h"

int main(int argc, char* argv[]) {
    ember::CmdLineArgs cmdLineArgs{ argc, argv };
    ember::EmberLvlEditorApp app{ cmdLineArgs };
    app.Initialize();
    int exitCode = app.Run();
    app.Terminate();
    return exitCode;
}