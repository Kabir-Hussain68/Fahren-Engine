#include "editorLayer.h"

#include <imgui.h>
#include <ImGuizmo.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/scene/sceneSerializer.h"

#include "Engine/utils/platformUtils.h"
#include "Engine/math/math.h"

extern const std::filesystem::path g_AssetPath;

EditorLayer::EditorLayer()
    : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f)
{
}

void EditorLayer::onAttach()
{
    FH_PROFILE_FUNCTION();

    m_FaceTexture = Texture2D::create("assets/textures/face.png");

    FrameBufferSpecification fbSpec;
    fbSpec.attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RED_INTEGER, FrameBufferTextureFormat::Depth };
    fbSpec.width = 1280;
    fbSpec.height = 720;
    m_FrameBuffer = FrameBuffer::create(fbSpec);

    m_ActiveScene = createRef<Scene>();

    auto commandLineArgs = Application::getApplication().getCommandLineArgs();
	if (commandLineArgs.count > 1)
	{
		auto sceneFilePath = commandLineArgs[1];
		SceneSerializer serializer(m_ActiveScene);
		serializer.deserialize(sceneFilePath);
	}

    m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

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
        m_EditorCamera.setViewportSize(m_ViewportSize.x, m_ViewportSize.y);

        m_ActiveScene->onViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    }

    if (m_ViewportFocused)
    {
        m_CameraController.onUpdate(ts);
        m_EditorCamera.onUpdate(ts);
    }


    Renderer2D::resetStats();

    {
        FH_PROFILE_SCOPE("Renderer Prep");
        m_FrameBuffer->bind();
        RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        RenderCommand::clear();

        //Clear entity attachment to -1
        m_FrameBuffer->clearAttachments(1, -1);
    }
    
    {
        FH_PROFILE_SCOPE("Renderer Draw");
        
        m_ActiveScene->onUpdateEditor(ts, m_EditorCamera);

        auto[mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        my = viewportSize.y -my;

        int mouseX = (int)mx;
        int mouseY = (int)my;

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
        {
            FH_PROFILE_SCOPE("Mouse picking readPixel");

            int pixelData = m_FrameBuffer->readPixel(1, mouseX, mouseY);
            if (pixelData == -1)
                m_HoveredEntity = {};
            else
                m_HoveredEntity = { entt::entity(pixelData), m_ActiveScene.get() };
        }   

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
    m_ContentBrowserPanel.onImGuiRender();

    ImGui::Begin("Stats");

    std::string name = "None";
    if (m_HoveredEntity)
        name = m_HoveredEntity.getComponent<TagComponent>().tag;
    ImGui::Text("Hovered Entity : %s", name.c_str());

    auto stats = Renderer2D::getStats();
    ImGui::Text("Renderer2D Stats : ");
    ImGui::Text("Draw Calls : %d", stats.drawCalls);
    ImGui::Text("Quads : %d", stats.quadCount);
    ImGui::Text("Vertices : %d", stats.getTotalVertexCount());
    ImGui::Text("Indices : %d", stats.getTotalIndexCount());

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");

    auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
    auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    auto viewportOffset = ImGui::GetWindowPos(); //Includes Tab Bar
    m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

    m_ViewportFocused = ImGui::IsWindowFocused();
    m_ViewportHovered = ImGui::IsWindowHovered();
    Application::getApplication().getImGuiLayer()->blockEvents(!m_ViewportFocused && !m_ViewportHovered);

    ImVec2 viewPortPanelSize = ImGui::GetContentRegionAvail();
    m_ViewportSize = { viewPortPanelSize.x, viewPortPanelSize.y };

    uint64_t textureID = m_FrameBuffer->getColorAttachmentRendererID();
    ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2(m_ViewportSize.x, m_ViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload("Content_Browser_Item"))
        {
            const char* path = (const char*)payLoad->Data;
            std::filesystem::path scenePath = std::filesystem::path(g_AssetPath) / path;
            if (scenePath.extension() == ".fahren")
            {
                openScene(scenePath);
            }
            else
            {
                FH_CORE_WARN("Cannot drop file type '{0}' onto viewport", scenePath.extension().string());
            }
            }
        ImGui::EndDragDropTarget(); 
    }

    //Gizmos
    Entity selectedEntity = m_SceneHierarchyPanel.getSelectedEntity();
    if (selectedEntity && m_GizmoType != -1)
    {
        ImGuizmo::SetOrthographic(false);   
        ImGuizmo::SetDrawlist();
        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

        //Camera

        //Runtime Camer
        // auto cameraEntity = m_ActiveScene->getPrimaryCameraEntity();
        // const auto& camera = cameraEntity.getComponent<CameraComponent>().camera;
        // const glm::mat4& cameraProjection = camera.getProjection();
        // glm::mat4 cameraView = glm::inverse(cameraEntity.getComponent<TransformComponent>().getTransform());

        //Editor Camera
        const glm::mat4& cameraProjection = m_EditorCamera.getProjection();
        glm::mat4 cameraView = m_EditorCamera.getViewMatrix();

        //Entity Transform
        auto& tc = selectedEntity.getComponent<TransformComponent>();
        glm::mat4 transform = tc.getTransform();

        //Snapping
        bool snap = Input::isKeyPressed(Key::LeftShift);
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
    m_EditorCamera.onEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.dispatch<KeyPressedEvent>(FH_BIND_EVENT_FN(EditorLayer::onKeyPressed));
    dispatcher.dispatch<MouseButtonPressedEvent>(FH_BIND_EVENT_FN(EditorLayer::onMouseButtonPressed));
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

bool EditorLayer::onMouseButtonPressed(MouseButtonPressedEvent &event)
{
    if (event.getMouseButton() == Mouse::ButtonLeft)
    {
        if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::isKeyPressed(Key::LeftControl))
            m_SceneHierarchyPanel.setSelectedEntity(m_HoveredEntity);
    }
    return false;
}

void EditorLayer::newScene()
{
    m_ActiveScene = createRef<Scene>();
    m_ActiveScene->onViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    m_SceneHierarchyPanel.setContext(m_ActiveScene);
}

void EditorLayer::openScene(const std::filesystem::path& path)
{
    m_ActiveScene = createRef<Scene>();
    m_ActiveScene->onViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    m_SceneHierarchyPanel.setContext(m_ActiveScene);

    SceneSerializer serializer(m_ActiveScene);
    serializer.deserialize(path.string());   
}

void EditorLayer::openScene()
{
    std::string filePath = FileDialogs::openFile("Fahren Scene (*.fahren)\0*.fahren\0");
    if (!filePath.empty())
    {
        openScene(filePath);
    }
}


void EditorLayer::saveSceneAs()
{
    std::string filePath = FileDialogs::saveFile("Fahren Scene (*.fahren)\0*.fahren\0");
    if (!filePath.empty())
    {
        SceneSerializer serializer(m_ActiveScene);
        serializer.serialize(filePath);
    }
}

