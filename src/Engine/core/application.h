#pragma once

#include "window.h"
#include "Engine/core/layerStack.h"
#include "Engine/events/events.h"
#include "Engine/events/applicationEvent.h"

#include "Engine/imGui/imGuiLayer.h"

#include "Engine/renderer/shader.h"

class Application
{
private:
    static Application* m_Instance;

    std::unique_ptr<Window> m_Window;
    bool m_Running = true;
    
    ImGuiLayer* m_ImGuiLayer;
    LayerStack m_LayerStack;

    unsigned int m_VertexArray;
    unsigned int m_VertexBuffer;
    unsigned int m_IndexBuffer;

    std::unique_ptr<Shader> m_Shader;

    bool onWindowClose(WindowCloseEvent& e);
public:
    Application();
    ~Application();

    void run();

    void onEvent(Event& e);

    void pushLayer(Layer* layer);
    void pushOverlay(Layer* layer);

    inline static Application& getApplication() { return *m_Instance; }
    inline Window& getWindow() { return *m_Window; }
};