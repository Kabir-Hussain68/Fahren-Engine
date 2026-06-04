#include "application.h"

#include "fhpch.h"

#include "Engine/renderer/renderer.h"

#include "GLFW/glfw3.h"

Application* Application::m_Instance = nullptr;

Application::Application()
{
    FH_CORE_ASSERT(!m_Instance, "Application already exists");
    m_Instance = this;

    m_Window = std::unique_ptr<Window>(Window::createWindow());
    m_Window->setEventCallback(FH_BIND_EVENT_FN(onEvent));

    Renderer::Init();

    m_ImGuiLayer = new ImGuiLayer();
    pushOverlay(m_ImGuiLayer);

}

Application::~Application()
{
}

void Application::pushLayer(Layer* layer)
{
    m_LayerStack.pushLayer(layer);
    layer->onAttach();
}

void Application::pushOverlay(Layer* layer)
{
    m_LayerStack.pushOverlay(layer);
    layer->onAttach();
}

void Application::onEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(FH_BIND_EVENT_FN(onWindowClose));

    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
    {
        (*--it)->onEvent(e);
        if (e.Handled)
            break;

    }
}

void Application::run()
{
    while(m_Running)
    {
        float time = (float)glfwGetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime = time;

        for (Layer* layer : m_LayerStack)
        {
            layer->onUpdate(timestep);
        }

        m_ImGuiLayer->begin();
        for (Layer* layer : m_LayerStack)
        {
            layer->onImGuiRender();
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
