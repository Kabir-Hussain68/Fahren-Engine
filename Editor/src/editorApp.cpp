#include <Fahren.h>
#include <Engine/core/entryPoint.h>

#include "editorLayer.h"

class ExampleLayer : public Layer
{
public:

};

class Editor : public Application
{
public:
    Editor()
        : Application("Fahren Editor")
    {
        pushLayer(new EditorLayer());
    }

    ~Editor() {}
};

Application* createApplication()
{
    return new Editor();
}