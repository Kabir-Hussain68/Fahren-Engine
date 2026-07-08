#pragma once

#include "application.h"

#if defined(FH_PLATFORM_LINUX) || defined(FH_PLATFORM_WINDOWS) || defined(FH_PLATFORM_MAC)

extern Application* createApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
    Log::Init();

    FH_PROFILE_THREAD("Main Thread");

    Application* app = createApplication({ argc, argv });

    app->run();

    delete app;
}

#endif

