#pragma once

#include "Fahren.h"
#include "panels/sceneHierarchyPanel.h"

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
    Entity m_CameraEntity;
    Entity m_SeconcCamera;

    bool m_PrimaryCamera = true;

    bool m_ViewportFocused = false;
    bool m_ViewportHovered = false;
    glm::vec2 m_ViewportSize = {0.0f, 0.0f}; 

    //Gizmos
    int m_GizmoType = -1;

    //Panels
    SceneHierarchyPanel m_SceneHierarchyPanel;

    bool onKeyPressed(KeyPressedEvent& event);

    void newScene();
    void saveSceneAs();
    void openScene();

public:
    EditorLayer();
    virtual ~EditorLayer() = default;

    virtual void onAttach() override;
    virtual void onDetach() override;

    void onUpdate(Timestep ts) override;
    virtual void onImGuiRender() override;
    void onEvent(Event& event) override;
};