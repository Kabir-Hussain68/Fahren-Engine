#pragma once

#include "window.h"
#include "Engine/core/layerStack.h"
#include "Engine/events/events.h"
#include "Engine/events/applicationEvent.h"
#include "Engine/core/timestep.h"

#include "Engine/imGui/imGuiLayer.h"

class Application
{
private:
    static Application* m_Instance;

    std::unique_ptr<Window> m_Window;
    bool m_Running = true;
    bool m_Minimized = false;
    
    ImGuiLayer* m_ImGuiLayer;
    LayerStack m_LayerStack;

    float m_LastFrameTime = 0.0f;

    bool onWindowClose(WindowCloseEvent& e);
    bool onWindowResize(WindowResizeEvent& e);
public:
    Application();
    ~Application();

    void run();

    void onEvent(Event& e);

    void pushLayer(Layer* layer);
    void pushOverlay(Layer* layer);

    inline static Application& getApplication() { return *m_Instance; }
    inline Window& getWindow() { return *m_Window; }

    void close();
};