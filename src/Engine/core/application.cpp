#include "application.h"

#include "fhpch.h"

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application::Application()
{
    m_Window = std::unique_ptr<Window>(Window::createWindow());
    m_Window->setEventCallback(BIND_EVENT_FN(onEvent));
}

Application::~Application()
{
}

void Application::pushLayer(Layer* layer)
{
    m_LayerStack.pushLayer(layer);
}

void Application::pushOverlay(Layer* layer)
{
    m_LayerStack.pushOverlay(layer);
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
