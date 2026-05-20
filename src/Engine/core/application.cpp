#include "application.h"

#include "fhpch.h"

#include <glad/glad.h>

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application* Application::m_Instance = nullptr;

Application::Application()
{
    FH_CORE_ASSERT(!m_Instance, "Application already exists");
    m_Instance = this;

    m_Window = std::unique_ptr<Window>(Window::createWindow());
    m_Window->setEventCallback(BIND_EVENT_FN(onEvent));
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
    dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(onWindowClose));

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
        glClear(GL_COLOR_BUFFER_BIT);

        for (Layer* layer : m_LayerStack)
        {
            layer->onUpdate();
        }

        m_Window->onUpdate();
    }
}

bool Application::onWindowClose(WindowCloseEvent& e)
{
    m_Running = false;
    return true;
}
