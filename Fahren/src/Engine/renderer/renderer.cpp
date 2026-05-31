#include "fhpch.h"
#include "renderer.h"

Renderer::sceneData* Renderer::m_SceneData = new Renderer::sceneData;

void Renderer::beginScene(orthographicCamera& camera)
{
    m_SceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
}

void Renderer::endScene()
{

}

void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray> &vertexArray)
{
    shader->bind();
    shader->uploadUniform("u_ViewProjection", m_SceneData->ViewProjectionMatrix);

    vertexArray->bind();
    RenderCommand::drawIndexed(vertexArray);


}
