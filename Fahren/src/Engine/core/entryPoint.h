#pragma once

#include "application.h"

#if defined(FH_PLATFORM_LINUX) || defined(FH_PLATFORM_WINDOWS) || defined(FH_PLATFORM_MAC)

extern Application* createApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
    Log::Init();

    FH_PROFILE_BEGIN_SESSION("Statup", "FahrenProfile-Startup.json");
    Application* app = createApplication({ argc, argv });
    FH_PROFILE_END_SESSION();

    FH_PROFILE_BEGIN_SESSION("Runtime", "FahrenProfile-Runtime.json");
    app->run();
    FH_PROFILE_END_SESSION();

    FH_PROFILE_BEGIN_SESSION("Shutdown", "FahrenProfile-Shutdown.json");
    delete app;
    FH_PROFILE_END_SESSION();
}

#endif

