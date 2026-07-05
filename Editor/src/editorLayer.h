#pragma once

#include "Fahren.h"
#include "panels/sceneHierarchyPanel.h"
#include "panels/contentBrowserPanel.h"

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

    //EntityID
    Entity m_HoveredEntity;

    bool m_PrimaryCamera = true;

    EditorCamera m_EditorCamera;

    bool m_ViewportFocused = false;
    bool m_ViewportHovered = false;
    glm::vec2 m_ViewportSize = {0.0f, 0.0f}; 
    glm::vec2 m_ViewportBounds[2];

    //Gizmos
    int m_GizmoType = -1;

    //Panels
    SceneHierarchyPanel m_SceneHierarchyPanel;
    ContentBrowserPanel m_ContentBrowserPanel;


    bool onKeyPressed(KeyPressedEvent& event);
    bool onMouseButtonPressed(MouseButtonPressedEvent& event);

    void newScene();
    void saveSceneAs();
    void openScene(const std::filesystem::path& path);
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