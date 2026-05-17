#include "Engine/core/application.h"
#include "Engine/core/log.h"

int main() 
{
    Log::Init();
    FH_CORE_INFO("Initialized Log");

    Application* app = new Application();
    app->run();
    delete app;
}