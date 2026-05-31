#pragma once

#include "renderCommand.h"
#include "orthographicCamera.h"
#include "shader.h"

class Renderer
{
private:
    struct sceneData
    {
        glm::mat4 ViewProjectionMatrix;
    };

    static sceneData* m_SceneData;

public:
    static void beginScene(orthographicCamera& camera);
    static void endScene();

    static void submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray);

    inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
};
