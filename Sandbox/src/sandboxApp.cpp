#include <Fahren.h>
#include "Engine/core/entryPoint.h"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sandbox2D.h"

class ExampleLayer : public Layer
{
public:

};

class Sandbox : public Application
{
public:
    Sandbox(ApplicationCommandLineArgs args)
        : Application("Fahren Test", args)
    {
        pushLayer(new Sandbox2D());
    }

    ~Sandbox() {}
};

Application* createApplication(ApplicationCommandLineArgs args)
{
    return new Sandbox(args);
}