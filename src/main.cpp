#include <Fahren.h>

class ExampleLayer : public Layer
{
public:
    ExampleLayer()
        : Layer("Eampler") {}
    
    void onUpdate()
    {
        FH_INFO("Exampler Layer Update");
    }

    void onEvent(Event& event)
    {
        FH_CORE_TRACE("{0}", event.toString());
    }
};

int main() 
{
    Log::Init();
    FH_CORE_INFO("Initialized Log");

    Application* app = new Application();
    app->pushLayer(new ImGuiLayer());
    app->run();
    delete app;
}