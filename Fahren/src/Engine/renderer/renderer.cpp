#include "fhpch.h"
#include "renderer.h"

#include "Platform/OpenGL/openGLShader.h"

Renderer::sceneData* Renderer::m_SceneData = new Renderer::sceneData;

void Renderer::beginScene(orthographicCamera& camera)
{
    m_SceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
}

void Renderer::endScene()
{

}

void Renderer::submit(const Ref<Shader>& shader, const Ref<VertexArray> &vertexArray, const glm::mat4& transform)
{
    shader->bind();
    std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadUniformMat4("u_ViewProjection", m_SceneData->ViewProjectionMatrix);
    std::dynamic_pointer_cast<OpenGLShader>(shader)->uploadUniformMat4("u_Transform", transform);

    vertexArray->bind();
    RenderCommand::drawIndexed(vertexArray);


}
