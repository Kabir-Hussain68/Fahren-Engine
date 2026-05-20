#pragma once

#if defined(FH_PLATFORM_LINUX) || defined(FH_PLATFORM_WINDOWS) || defined(FH_PLATFORM_MAC)

extern Application* createApplication();

int main()
{
    Log::Init();
    FH_CORE_INFO("Initialized Log");

    Application* app = createApplication();
    app->run();
    delete app;
}

#endif

