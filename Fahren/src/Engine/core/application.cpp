#include "application.h"

#include "fhpch.h"

#include "Engine/renderer/renderer.h"

#include "GLFW/glfw3.h"

Application* Application::m_Instance = nullptr;

Application::Application()
{
    FH_PROFILE_FUNCTION();


    FH_CORE_ASSERT(!m_Instance, "Application already exists");
    m_Instance = this;

    m_Window = Window::createWindow();
    m_Window->setEventCallback(FH_BIND_EVENT_FN(onEvent));

    Renderer::Init();

    m_ImGuiLayer = new ImGuiLayer();
    pushOverlay(m_ImGuiLayer);

}

Application::~Application()
{
    FH_PROFILE_FUNCTION();

    Renderer::shutdown();
}

void Application::pushLayer(Layer* layer)
{
    FH_PROFILE_FUNCTION();

    m_LayerStack.pushLayer(layer);
    layer->onAttach();
}

void Application::pushOverlay(Layer* layer)
{
    FH_PROFILE_FUNCTION();

    m_LayerStack.pushOverlay(layer);
    layer->onAttach();
}

void Application::onEvent(Event& e)
{
    FH_PROFILE_FUNCTION();

    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(FH_BIND_EVENT_FN(onWindowClose));
    dispatcher.dispatch<WindowResizeEvent>(FH_BIND_EVENT_FN(onWindowResize));

    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
    {
        (*--it)->onEvent(e);
        if (e.Handled)
            break;

    }
}

void Application::run()
{
    FH_PROFILE_FUNCTION();

    while(m_Running)
    {
        FH_PROFILE_SCOPE("RunLoop");

        float time = (float)glfwGetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime = time;

        if(!m_Minimized)
        {
            {
                FH_PROFILE_SCOPE("LayerStack onUpdate");

                for (Layer* layer : m_LayerStack)
                {
                    layer->onUpdate(timestep);
                }
            }
        }
        
        m_ImGuiLayer->begin();
        {
            FH_PROFILE_SCOPE("LayerStack onImGuiRender");

            for (Layer* layer : m_LayerStack)
            {
                layer->onImGuiRender();
            }
        }
        m_ImGuiLayer->end();

        m_Window->onUpdate();
    }
}

bool Application::onWindowClose(WindowCloseEvent& e)
{
    m_Running = false;
    return true;
}

bool Application::onWindowResize(WindowResizeEvent& e)
{
    FH_PROFILE_FUNCTION();
    
    if (e.getWidth() == 0 || e.getHeight() == 0)
    {
        m_Minimized = true;
        return false;
    }
    
    m_Minimized = false;
    Renderer::onWindowResize(e.getWidth(), e.getHeight());

    return false;
}
