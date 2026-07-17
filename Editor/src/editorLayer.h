#pragma once

#include "Fahren.h"
#include "panels/sceneHierarchyPanel.h"
#include "panels/contentBrowserPanel.h"

class EditorLayer : public Layer
{
private:
    enum class SceneState
    {
        Edit = 0, Play = 1, Simulate = 2
    };

    SceneState m_SceneState = SceneState::Edit;

    Ref<FrameBuffer> m_FrameBuffer;

    Ref<Scene> m_ActiveScene;
    Ref<Scene> m_Editorscene;
    std::filesystem::path m_EditorScenePath;

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

    //UI
    Ref<Texture2D> m_IconPlay;
    Ref<Texture2D> m_IconSimulate;
    Ref<Texture2D> m_IconStop;

    bool m_ShowPhysicsCollider = false;

    //Panels
    SceneHierarchyPanel m_SceneHierarchyPanel;
    ContentBrowserPanel m_ContentBrowserPanel;

    bool onKeyPressed(KeyPressedEvent& event);
    bool onMouseButtonPressed(MouseButtonPressedEvent& event);

    void onOverlayRender();

    void newScene();
    void saveScene();
    void saveSceneAs();
    void openScene(const std::filesystem::path& path);
    void openScene();

    void serializeScene(Ref<Scene> scene, const std::filesystem::path& path);

    void onScenePlay();
    void onSceneSimulate();
    void onSceneStop();

    void onDeleteEntity();
    void onDuplicateEntity();

public:
    EditorLayer();
    virtual ~EditorLayer() = default;

    virtual void onAttach() override;
    virtual void onDetach() override;

    void onUpdate(Timestep ts) override;
    virtual void onImGuiRender() override;
    void onEvent(Event& event) override;

    // UI Panels
    void UI_Toolbar();
};