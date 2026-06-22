#pragma once

#include "Fahren.h"

class EditorLayer : public Layer
{
private:
    OrthographicCameraController m_CameraController;

    Ref<Shader> m_FlatColorShader;
    Ref<VertexArray> m_SquareVA;
    Ref<FrameBuffer> m_FrameBuffer;

    Ref<Texture2D> m_FaceTexture;

    Ref<Scene> m_ActiveScene;
    Entity m_SquareEntity;

    bool m_ViewportFocused = false;
    bool m_ViewportHovered = false;
    glm::vec2 m_ViewportSize = {0.0f, 0.0f}; 

    glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f};

public:
    EditorLayer();
    virtual ~EditorLayer() = default;

    virtual void onAttach() override;
    virtual void onDetach() override;

    void onUpdate(Timestep ts) override;
    virtual void onImGuiRender() override;
    void onEvent(Event& event) override;
};