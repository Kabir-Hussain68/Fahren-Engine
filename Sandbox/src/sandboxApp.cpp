#include <Fahren.h>

class ExampleLayer : public Layer
{
public:
    ExampleLayer()
        : Layer("Example Layer") {}
    
    void onUpdate() override {}
    void onEvent(Event& event) override {}
};

class Sandbox : public Application
{
public:
    Sandbox()
    {
        pushLayer(new ExampleLayer());
    }

    ~Sandbox() {}
};

Application* createApplication()
{
    return new Sandbox();
}