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
    Editor(ApplicationCommandLineArgs args)
        : Application("Fahren Editor", args)
    {
        pushLayer(new EditorLayer());
    }

    ~Editor() {}
};

Application* createApplication(ApplicationCommandLineArgs args)
{
    return new Editor(args);
}