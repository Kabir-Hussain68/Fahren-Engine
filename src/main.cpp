#include <Fahren.h>

int main() 
{
    Log::Init();
    FH_CORE_INFO("Initialized Log");

    Application* app = new Application();
    app->run();
    delete app;
}