#pragma once

#include "window.h"
#include "Engine/core/layerStack.h"
#include "Engine/events/events.h"
#include "Engine/events/applicationEvent.h"
#include "Engine/core/timestep.h"

#include "Engine/imGui/imGuiLayer.h"

struct ApplicationCommandLineArgs
{
    int count = 0;
    char** args = nullptr;

    const char* operator[](int index) const
    {
        FH_CORE_ASSERT(index < count);
        return args[index];
    }
};

int main(int argc, char** argv);

class Application
{
private:
    friend int main(int argc, char** argv);
    ApplicationCommandLineArgs m_CommandLineArgs;

    static Application* m_Instance;

    Scope<Window> m_Window;
    bool m_Running = true;
    bool m_Minimized = false;
    
    ImGuiLayer* m_ImGuiLayer;
    LayerStack m_LayerStack;

    float m_LastFrameTime = 0.0f;

    void run();
    
    bool onWindowClose(WindowCloseEvent& e);
    bool onWindowResize(WindowResizeEvent& e);
public:
    Application(const std::string& name = "Fahren Engine", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
    virtual ~Application();

    void onEvent(Event& e);

    void pushLayer(Layer* layer);
    void pushOverlay(Layer* layer);

    inline static Application& getApplication() { return *m_Instance; }
    inline Window& getWindow() { return *m_Window; }
    ApplicationCommandLineArgs getCommandLineArgs() const { return m_CommandLineArgs; }
    ImGuiLayer* getImGuiLayer() { return m_ImGuiLayer; };

    void close();
};

// To be defined in CLIENT
Application* createApplication(ApplicationCommandLineArgs args);