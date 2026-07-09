#include <Fahren.h>
#include <Engine/core/entryPoint.h>

#include "editorLayer.h"

class Editor : public Application
{
public:
    // Creates a new layer called "Fahren Editor"
    // Pushes this application as layer to the stack for execution
    Editor(ApplicationCommandLineArgs args)
        : Application("Fahren Editor", args)
    {
        pushLayer(new EditorLayer());
    }

    // Empty destructor
    ~Editor() {}
};

// Called in entry point as this is client (editor)
Application* createApplication(ApplicationCommandLineArgs args)
{
    return new Editor(args);
}