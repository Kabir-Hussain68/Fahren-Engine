#pragma once

#include "Fahren.h"

class Sandbox2D : public Layer
{
private:
    OrthographicCameraController m_CameraController;

    Ref<Shader> m_FlatColorShader;
    Ref<VertexArray> m_SquareVA;

    Ref<Texture2D> m_FaceTexture;

    glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f};

public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;

    virtual void onAttach() override;
    virtual void onDetach() override;

    void onUpdate(Timestep ts) override;
    virtual void onImGuiRender() override;
    void onEvent(Event& event) override;
};