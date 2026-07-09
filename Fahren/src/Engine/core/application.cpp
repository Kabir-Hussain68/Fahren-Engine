#include "application.h"

#include "fhpch.h"

#include "Engine/renderer/renderer.h"

#include "GLFW/glfw3.h"

Application* Application::m_Instance = nullptr;

Application::Application(const std::string& name, ApplicationCommandLineArgs args)
{
    FH_PROFILE_FUNCTION();


    FH_CORE_ASSERT(!m_Instance, "Application already exists");
    m_Instance = this;


    //Create window and setup event callback
    //Initialize the renderer and setup ImGui Layer
    m_Window = Window::createWindow(windowProps(name));
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
    m_LayerStack.pushLayer(layer);
    layer->onAttach();
}

void Application::pushOverlay(Layer* layer)
{
    m_LayerStack.pushOverlay(layer);
    layer->onAttach();
}

void Application::close()
{
    m_Running = false;
}

void Application::onEvent(Event& e)
{
    FH_PROFILE_FUNCTION();

    //Handles if the window is closed or resized
    //It goes through all the events in all the layers and handles them, by calling onEvent
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(FH_BIND_EVENT_FN(onWindowClose));
    dispatcher.dispatch<WindowResizeEvent>(FH_BIND_EVENT_FN(onWindowResize));

    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
    {
        if (e.Handled)
            break;
        (*it)->onEvent(e);

    }
}

void Application::run()
{
    // Main Loop of the game engine
    while(m_Running)
    {
        FH_PROFILE_SCOPE("RunLoop");

        float time = (float)glfwGetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime = time;

        // If the window is minimized stop all the layers
        // If thw window is not minimized continue running the onupdate 
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
        
        // Start the ImGui window, loop through all the layers, if ImGui Layer, render it
        // Begin initializes ImGui starts the new frame, end renders the frame and sets the context
        m_ImGuiLayer->begin();
        {
            FH_PROFILE_SCOPE("LayerStack onImGuiRender");

            for (Layer* layer : m_LayerStack)
            {
                layer->onImGuiRender();
            }
        }
        m_ImGuiLayer->end();

        // Runs the window on update
        // Swaps buffers and polls events
        m_Window->onUpdate();

        FH_PROFILE_FRAME();
    }
}

// If window is terminated
bool Application::onWindowClose(WindowCloseEvent& e)
{
    m_Running = false;
    return true;
}

// Handles the case of window minimized 
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
