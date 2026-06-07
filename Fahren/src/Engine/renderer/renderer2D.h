#pragma once

#include "orthographicCamera.h"
#include "texture.h"

class Renderer2D
{
public:
    static void Init();
    static void shutdown();

    static void beginScene(const OrthographicCamera& camera);
    static void endScene();

    static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
    static void drawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture);
    static void drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture);
};