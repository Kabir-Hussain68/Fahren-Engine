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
    Sandbox()
    {
        pushLayer(new Sandbox2D());
    }

    ~Sandbox() {}
};

Application* createApplication()
{
    return new Sandbox();
}