#include "Core/CmdLineArgsParser.h"
#include "EmberLvlEditorApp.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[]) {
    ember::CmdLineArgsParser cmdLineArgsParser{};
    try {
        cmdLineArgsParser.Parse(argc, argv);
    } catch (std::runtime_error& re) {
        std::cerr << "Command Line Arguments Parser Error: " << re.what() << std::endl;
        cmdLineArgsParser.PrintHelp();
        return EXIT_FAILURE;
    }
    ember::EmberLvlEditorApp app{cmdLineArgsParser.GetCmdLineArgs()};
    if (!app.Initialize()) {
        return EXIT_FAILURE;
    }
    int exitCode = app.Run();
    app.Terminate();
    return exitCode;
}