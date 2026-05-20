#include <Fahren.h>

class ExampleLayer : public Layer
{
public:
    ExampleLayer()
        : Layer("Exampler") {}
    
    void onUpdate() override {}
    void onEvent(Event& event) override {}
};

class Sandbox : public Application
{
public:
    Sandbox()
    {
        pushLayer(new ImGuiLayer());
    }

    ~Sandbox() {}
};

Application* createApplication()
{
    return new Sandbox();
}