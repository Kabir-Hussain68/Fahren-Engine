#include "editorLayer.h"

#include <imgui.h>
#include <ImGuizmo.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/scene/sceneSerializer.h"

#include "Engine/utils/platformUtils.h"
#include "Engine/math/math.h"

EditorLayer::EditorLayer()
    : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
{
}

void EditorLayer::onAttach()
{
    FH_PROFILE_FUNCTION();

    m_FaceTexture = Texture2D::create("assets/textures/face.png");

    FrameBufferSpecification fbSpec;
    fbSpec.width = 1280;
    fbSpec.height = 720;
    m_FrameBuffer = FrameBuffer::create(fbSpec);

    m_ActiveScene = createRef<Scene>();

#if 0

    auto redSquare = m_ActiveScene->createEntity("Red Square");
    redSquare.addComponent<SpriteRendererComponent>(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    auto greenSquare = m_ActiveScene->createEntity("Green Square");
    greenSquare.addComponent<SpriteRendererComponent>(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

    m_SquareEntity = redSquare;

    m_CameraEntity = m_ActiveScene->createEntity("Camera A");
    m_CameraEntity.addComponent<CameraComponent>();

    m_SeconcCamera = m_ActiveScene->createEntity("Camera B");
    auto& cc = m_SeconcCamera.addComponent<CameraComponent>();
    cc.primary = false;

    class Test : public ScriptableEntity
    {
    public:
        void onCreate()
        {
        }

        void onDestroy()
        {

        }

        void onUpdate(Timestep ts)
        {
            auto& translation = getComponent<TransformComponent>().translation;

            if (Input::isKeyPressed(KeyCode::A))
                translation.x -= 1.0f * ts;
            if (Input::isKeyPressed(KeyCode::D))
                translation.x += 1.0f * ts;
            if (Input::isKeyPressed(KeyCode::W))
                translation.y += 1.0f * ts;
            if (Input::isKeyPressed(KeyCode::S))
                translation.y -= 1.0f * ts;
        }
    };

    m_CameraEntity.addComponent<NativeScriptComponent>().bind<Test>();
#endif

    m_SceneHierarchyPanel.setContext(m_ActiveScene);
}

void EditorLayer::onDetach()
{
    FH_PROFILE_FUNCTION();
}

void EditorLayer::onUpdate(Timestep ts)
{
    FH_PROFILE_FUNCTION();

    if (FrameBufferSpecification spec = m_FrameBuffer->getSpecification();
        m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
            (spec.width != m_ViewportSize.x || spec.height != m_ViewportSize.y))
    {
        m_FrameBuffer->resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_CameraController.onResize(m_ViewportSize.x, m_ViewportSize.y);

        m_ActiveScene->onViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    }

    if (m_ViewportFocused)
    {
        m_CameraController.onUpdate(ts);
    }

    Renderer2D::resetStats();

    {
        FH_PROFILE_SCOPE("Renderer Prep");
        m_FrameBuffer->bind();
        RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 0.1f});
        RenderCommand::clear();
    }

    
    {
        FH_PROFILE_SCOPE("Renderer Draw");
        
        m_ActiveScene->onUpdate(ts);

        m_FrameBuffer->unBind();
    }

}

void EditorLayer::onImGuiRender()
{
    FH_PROFILE_FUNCTION();

    static bool dockSpaceOpen = true;
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    ImGuiIO& io = ImGui::GetIO();
    //io.FontGlobalScale = 2.0f;
    ImGuiStyle& style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    style.WindowMinSize.x = minWinSizeX;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "Ctrl+N"))
            {
                newScene();
            }

            if (ImGui::MenuItem("Open...", "Ctrl+O"))
            {
               openScene();
            }

            if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
            {
               saveSceneAs();
            }

            if (ImGui::MenuItem("Exit"))
            {
                Application::getApplication().close();
            } 

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    m_SceneHierarchyPanel.onImGuiRender();

    ImGui::Begin("Stats");

    auto stats = Renderer2D::getStats();
    ImGui::Text("Renderer2D Stats : ");
    ImGui::Text("Draw Calls : %d", stats.drawCalls);
    ImGui::Text("Quads : %d", stats.quadCount);
    ImGui::Text("Vertices : %d", stats.getTotalVertexCount());
    ImGui::Text("Indices : %d", stats.getTotalIndexCount());

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();
    Application::getApplication().getImGuiLayer()->blockEvents(!m_ViewportFocused && !m_ViewportHovered);

    ImVec2 viewPortPanelSize = ImGui::GetContentRegionAvail();
    m_ViewportSize = { viewPortPanelSize.x, viewPortPanelSize.y };

    uint32_t textureID = m_FrameBuffer->getColorAttachmentRendererID();
    ImGui::Image((ImTextureID)(uintptr_t)textureID, ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));

    //Gizmos
    Entity selectedEntity = m_SceneHierarchyPanel.getSelectedEntity();
    if (selectedEntity && m_GizmoType != -1)
    {
        ImGuizmo::SetOrthographic(false);   
        ImGuizmo::SetDrawlist();
        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

        //Camera
        auto cameraEntity = m_ActiveScene->getPrimaryCameraEntity();
        const auto& camera = cameraEntity.getComponent<CameraComponent>().camera;
        const glm::mat4& cameraProjection = camera.getProjection();
        glm::mat4 cameraView = glm::inverse(cameraEntity.getComponent<TransformComponent>().getTransform());

        //Entity Transform
        auto& tc = selectedEntity.getComponent<TransformComponent>();
        glm::mat4 transform = tc.getTransform();

        //Snapping
        bool snap = Input::isKeyPressed(Key::LeftControl);
        float snapValue = 0.5f;
        if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
            snapValue = 45.0f;
        
        float snapValues[3] = {snapValue, snapValue, snapValue};

        ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
                  (ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
                   nullptr, snap ? snapValues : nullptr);

        if (ImGuizmo::IsUsing())
        {
            glm::vec3 translation, rotation, scale;
            Math::decomposeTransform(transform, translation, rotation, scale);

            glm::vec3 deltaRotation = rotation - tc.rotation;
            tc.translation = translation;
            tc.rotation += deltaRotation;
            tc.scale = scale;
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::End();
}

void EditorLayer::onEvent(Event &event)
{
    m_CameraController.onEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.dispatch<KeyPressedEvent>(FH_BIND_EVENT_FN(EditorLayer::onKeyPressed));

}

bool EditorLayer::onKeyPressed(KeyPressedEvent &event)
{
    if(event.getRepeatCount() > 0)
        return false;

    bool controlPressed = Input::isKeyPressed(Key::LeftControl) || Input::isKeyPressed(Key::RightControl);
    bool shiftPressed = Input::isKeyPressed(Key::LeftShift) || Input::isKeyPressed(Key::RightShift);
    switch (event.getKeyCode())
    {
        //File Dialogs
        case Key::N:
        {   
            if (controlPressed)
                newScene();
            break;
        }

        case Key::O:
        {   
            if (controlPressed)
                openScene();
            break;
        }

        case Key::S:
        {   
            if (controlPressed && shiftPressed)
                saveSceneAs();
            break;
        }

        //Gizmos
        case Key::Q:
        {
            if (!ImGuizmo::IsUsing())
                m_GizmoType = -1;
            break;
        }
        case Key::W:
        {
            if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
            break;
        }
        case Key::E:
        {
            if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
            break;
        }
        case Key::R:
        {
            if (!ImGuizmo::IsUsing())
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
            break;
        }
    }

    return false;
}

void EditorLayer::newScene()
{
    m_ActiveScene = createRef<Scene>();
    m_ActiveScene->onViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    m_SceneHierarchyPanel.setContext(m_ActiveScene);
}

void EditorLayer::openScene()
{
    std::string filePath = FileDialogs::openFile("Fahren Scene (*.fahren)\0*.fahren\0");
    if (!filePath.empty())
    {
        m_ActiveScene = createRef<Scene>();
        m_ActiveScene->onViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.setContext(m_ActiveScene);

        SceneSerializer serializer(m_ActiveScene);
        serializer.deserialize(filePath);
    }
}

void EditorLayer::saveSceneAs()
{
    std::string filePath = FileDialogs::saveFile("Fahren Scene (*.fahren)\0*,fahren\0");
    if (!filePath.empty())
    {
        SceneSerializer serializer(m_ActiveScene);
        serializer.serialize(filePath);
    }
}

