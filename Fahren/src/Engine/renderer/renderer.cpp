#include "fhpch.h"
#include "renderer.h"
#include "renderer2D.h"

#include "Platform/OpenGL/openGLShader.h"

Scope<Renderer::sceneData> Renderer::m_SceneData = createScope<Renderer::sceneData>();

// Initializes the renderCommand for passing commands to the API
// Initializes the 2D renderer of 2D rendering
void Renderer::Init()
{
    FH_PROFILE_FUNCTION();

    RenderCommand::Init();
    Renderer2D::Init();
}

void Renderer::shutdown()
{
	Renderer2D::shutdown();
}

void Renderer::onWindowResize(uint32_t width, uint32_t height)
{
    RenderCommand::setViewport(0, 0, width, height);
}

// Passes the data of the camera to the scene
void Renderer::beginScene(OrthographicCamera &camera)
{
    m_SceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
}

void Renderer::endScene()
{

}

// Binds the shader
// Binds the Vertex Array
// Draws the object by the vertex array
void Renderer::submit(const Ref<Shader>& shader, const Ref<VertexArray> &vertexArray, const glm::mat4& transform)
{
    shader->bind();
    shader->setMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
	shader->setMat4("u_Transform", transform);

    vertexArray->bind();
    RenderCommand::drawIndexed(vertexArray);


}
