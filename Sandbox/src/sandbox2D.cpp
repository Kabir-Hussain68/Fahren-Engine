#include "sandbox2D.h"

#include <imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
}

void Sandbox2D::onAttach()
{
    FH_PROFILE_FUNCTION();

    m_FaceTexture = Texture2D::create("Sandbox/assets/textures/face.png");
}

void Sandbox2D::onDetach()
{
    FH_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(Timestep ts)
{
    FH_PROFILE_FUNCTION();


    m_CameraController.onUpdate(ts);

    {
        FH_PROFILE_SCOPE("Renderer Prep");
        RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 0.1f});
        RenderCommand::clear();
    }

    {
        FH_PROFILE_SCOPE("Renderer Draw");
        Renderer2D::beginScene(m_CameraController.getCamera());
        Renderer2D::drawQuad({-1.0f, 0.0f, 0.0f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.0f});
        Renderer2D::drawQuad({0.5f, -0.5f, 0.0f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.0f});
        Renderer2D::drawRotatedQuad({0.5f, -0.5f, 0.1f}, {0.5f, 0.5f}, glm::radians(45.0f), m_FaceTexture, 2.0f, glm::vec4(0.8f, 0.8f, 0.2f, 1.0f));
        Renderer2D::endScene();
    }

}

void Sandbox2D::onImGuiRender()
{
    FH_PROFILE_FUNCTION();

    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::End();
}

void Sandbox2D::onEvent(Event &event)
{
    m_CameraController.onEvent(event);
}
