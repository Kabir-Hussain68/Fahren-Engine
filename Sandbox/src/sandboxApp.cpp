#include <Fahren.h>
#include "Engine/core/entryPoint.h"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "sandbox2D.h"

class ExampleLayer : public Layer
{
private:
    Ref<Shader> m_Shader;
    Ref<VertexArray> m_VertexArray;

    Ref<Shader> flatColorShader, m_TextureShader;
	Ref<VertexArray> m_SquareVA;

    Ref<Texture2D> m_Texture, m_FaceTexture;

    OrthographicCameraController m_CameraController;
    glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.8f};

public:
    ExampleLayer()
        : Layer("Example Layer"), m_CameraController(1280.0f / 720.0f)
        {
            //VAO
            m_VertexArray = VertexArray::create();

            float vertices[3 * 7] =
            {
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                0.0f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f
            };

            //VBO
            Ref<VertexBuffer> triangleVB;
            triangleVB = VertexBuffer::create(vertices, sizeof(vertices));
            BufferLayout layout = 
            {
                {ShaderDataType::Float3, "a_Position"},
                {ShaderDataType::Float4, "a_Color"}
            };

            triangleVB->setLayout(layout);
            //Adding vertex buffer
            m_VertexArray->addVertexBuffer(triangleVB);

            //EBO
            unsigned int indices[3] = {0, 1, 2};
            Ref<IndexBuffer> triangleIB;
            triangleIB = IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));
            m_VertexArray->setIndexBuffer(triangleIB);

            m_SquareVA = VertexArray::create();

            float squareVertices[5 * 4] = {
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
                 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
                 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
                -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
            };

            Ref<VertexBuffer> squareVB;
            squareVB = VertexBuffer::create(squareVertices, sizeof(squareVertices));
            squareVB->setLayout({
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float2, "a_TexCoord" }
            });
            m_SquareVA->addVertexBuffer(squareVB);

            uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
            Ref<IndexBuffer> squareIB;
            squareIB = IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
            m_SquareVA->setIndexBuffer(squareIB);


            std::string vertexSrc = R"(
                #version 330 core

                layout (location = 0) in vec3 a_Position;
                layout (location = 1) in vec4 a_Color;

                out vec3 v_Position;
                out vec4 v_Color;

                uniform mat4 u_ViewProjection;
                uniform mat4 u_Transform;

                void main()
                {
                    v_Position = a_Position;
                    v_Color = a_Color;
                    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
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

            m_Shader = Shader::create("vertexPosColor" , vertexSrc, fragmentSrc);

            std::string flatColorVertexShader = R"(
                #version 330 core
                
                layout(location = 0) in vec3 a_Position;

                uniform mat4 u_ViewProjection;
                uniform mat4 u_Transform;

                out vec3 v_Position;

                void main()
                {
                    v_Position = a_Position;
                    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
                }
            )";

            std::string flatColorFragmentShader = R"(
                #version 330 core
                
                layout(location = 0) out vec4 color;

                in vec3 v_Position;
                uniform vec3 u_Color;

                void main()
                {
                    color = vec4(u_Color, 1.0f);
                }
            )";

            flatColorShader = Shader::create("FlatColor", flatColorVertexShader, flatColorFragmentShader);

            m_TextureShader = Shader::create("Sandbox/assets/shaders/texture.glsl");

            m_Texture = Texture2D::create("Sandbox/assets/textures/Checkerboard.png");
            m_FaceTexture = Texture2D::create("Sandbox/assets/textures/face.png");

            m_TextureShader->bind();
            m_TextureShader->setInt("u_Texture", 0);
        }
    
    void onUpdate(Timestep ts) override
    {
        //Camera
        m_CameraController.onUpdate(ts);

        RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 0.1f});
        RenderCommand::clear();

        Renderer::beginScene(m_CameraController.getCamera());

        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

        flatColorShader->bind();
        flatColorShader->setFloat3("u_Color", m_SquareColor);

        for(int y = 0; y < 20; y++)
        {
            for(int x = 0; x < 20; x++)
            {
                glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
                glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
                Renderer::submit(flatColorShader, m_SquareVA, transform);
            }
        }

        m_Texture->bind();
        Renderer::submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
        m_FaceTexture->bind();
        Renderer::submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

        //Triangle
        //Renderer::submit(m_Shader, m_VertexArray);

        Renderer::endScene();
    }

    virtual void onImGuiRender() override
    {
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::End();
    }

    void onEvent(Event& event) override 
    {
        m_CameraController.onEvent(event);
    }
};

class Sandbox : public Application
{
public:
    Sandbox()
    {
        //pushLayer(new ExampleLayer());
        pushLayer(new Sandbox2D());
    }

    ~Sandbox() {}
};

Application* createApplication()
{
    return new Sandbox();
}