#include "application.h"

#include "fhpch.h"

#include "Engine/renderer/renderer.h"

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application* Application::m_Instance = nullptr;

Application::Application()
{
    FH_CORE_ASSERT(!m_Instance, "Application already exists");
    m_Instance = this;

    m_Window = std::unique_ptr<Window>(Window::createWindow());
    m_Window->setEventCallback(BIND_EVENT_FN(onEvent));

    m_ImGuiLayer = new ImGuiLayer();
    pushOverlay(m_ImGuiLayer);

    //VAO
    m_VertexArray.reset(VertexArray::create());

    float vertices[3 * 7] =
    {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.0f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f
    };

    //VBO
    m_VertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));
    BufferLayout layout = 
    {
        {ShaderDataType::Float3, "a_Position"},
        {ShaderDataType::Float4, "a_Color"}
    };

    m_VertexBuffer->setLayout(layout);
    //Adding vertex buffer
    m_VertexArray->addVertexBuffer(m_VertexBuffer);

    //EBO
    unsigned int indices[3] = {0, 1, 2};
    m_IndexBuffer.reset(IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
    m_VertexArray->setIndexBuffer(m_IndexBuffer);

    std::string vertexSrc = R"(
        #version 330 core

        layout (location = 0) in vec3 a_Position;
        layout (location = 1) in vec4 a_Color;

        out vec3 v_Position;
        out vec4 v_Color;

        void main()
        {
            v_Position = a_Position;
            v_Color = a_Color;
            gl_Position = vec4(a_Position, 1.0);
        }
    )";

    std::string fragmentSrc = R"(
        #version 330 core

        layout (location = 0) out vec4 color;

        in vec3 v_Position;
        in vec4 v_Color;

        void main()
        {
            color = v_Color;
        }
    )";

    m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
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
        RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 0.1f});
        RenderCommand::clear();

        Renderer::beginScene();

        m_Shader->bind();
        Renderer::submit(m_VertexArray);

        Renderer::endScene();

        for (Layer* layer : m_LayerStack)
        {
            layer->onUpdate();
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
