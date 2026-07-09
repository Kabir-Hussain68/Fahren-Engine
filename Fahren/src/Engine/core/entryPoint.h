#pragma once

#include "application.h"

#if defined(FH_PLATFORM_LINUX) || defined(FH_PLATFORM_WINDOWS) || defined(FH_PLATFORM_MAC)

extern Application* createApplication(ApplicationCommandLineArgs args);

// The entry point of the application
int main(int argc, char** argv)
{
    Log::Init();

    FH_PROFILE_THREAD("Main Thread");
    
    // Creates the Application
    Application* app = createApplication({ argc, argv });

    // Runs the application
    app->run();

    // Terminates the application
    delete app;
}

#endif

