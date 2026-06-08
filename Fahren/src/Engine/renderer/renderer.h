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

    static Scope<sceneData> m_SceneData;

public:
    static void Init();
    static void shutdown();

    static void onWindowResize(uint32_t width, uint32_t height);

    static void beginScene(OrthographicCamera& camera);
    static void endScene();

    static void submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

    inline static RendererAPI::API getAPI() { return RendererAPI::getAPI(); }
};
