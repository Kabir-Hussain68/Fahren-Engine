#include <Fahren.h>

class ExampleLayer : public Layer
{
private:
    std::shared_ptr<Shader> m_Shader;
    std::shared_ptr<VertexArray> m_VertexArray;

    std::shared_ptr<VertexBuffer> m_VertexBuffer;
    std::shared_ptr<IndexBuffer> m_IndexBuffer;

    orthographicCamera m_Camera;

    glm::vec3 m_CameraMovePosition;
    float m_CameraMoveSpeed = 0.1f;

    float m_CameraRotation = 0.0f;
    float m_CameraRotateSpeed = 2.0f;

public:
    ExampleLayer()
        : Layer("Example Layer"), m_Camera(-1.0f, 1.0f, -1.0f, 1.0f), m_CameraMovePosition(0.0f)
        {
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

                uniform mat4 u_ViewProjection;

                void main()
                {
                    v_Position = a_Position;
                    v_Color = a_Color;
                    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
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
    
    void onUpdate() override
    {
        RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 0.1f});
        RenderCommand::clear();

        if(Input::isKeyPressed(FH_KEY_LEFT))
        {
            m_CameraMovePosition.x -= m_CameraMoveSpeed;
        }
        if(Input::isKeyPressed(FH_KEY_RIGHT))
        {
            m_CameraMovePosition.x += m_CameraMoveSpeed;
        }
        if(Input::isKeyPressed(FH_KEY_UP))
        {
            m_CameraMovePosition.y += m_CameraMoveSpeed;
        }
        if(Input::isKeyPressed(FH_KEY_DOWN))
        {
            m_CameraMovePosition.y -= m_CameraMoveSpeed;
        }

        if(Input::isKeyPressed(FH_KEY_A))
        {
            m_CameraRotation += m_CameraRotateSpeed;
        }

        if(Input::isKeyPressed(FH_KEY_D))
        {
            m_CameraRotation -= m_CameraRotateSpeed;
        }

        m_Camera.setPosition(m_CameraMovePosition);
        m_Camera.setRotation(m_CameraRotation);

        Renderer::beginScene(m_Camera);

        Renderer::submit(m_Shader, m_VertexArray);

        Renderer::endScene();
    }

    void onEvent(Event& event) override 
    {
    }
};

class Sandbox : public Application
{
public:
    Sandbox()
    {
        pushLayer(new ExampleLayer());
    }

    ~Sandbox() {}
};

Application* createApplication()
{
    return new Sandbox();
}