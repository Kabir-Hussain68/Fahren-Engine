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
    : Layer("EditorLayer")
{
}

void EditorLayer::onAttach()
{
    FH_PROFILE_FUNCTION();

    m_IconPlay = Texture2D::create("resources/icons/play/play.png");
    m_IconSimulate = Texture2D::create("resources/icons/play/simulate.png");
    m_IconStop = Texture2D::create("resources/icons/play/stop.png");

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
        m_EditorCamera.setViewportSize(m_ViewportSize.x, m_ViewportSize.y);

        m_ActiveScene->onViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    }

    Renderer2D::resetStats();

    {
        FH_PROFILE_SCOPE("Renderer Prep");
        m_FrameBuffer->bind();
        RenderCommand::setClearColor({0.1f, 0.1f, 0.1f, 1.0f});
        RenderCommand::clear();

        //Clear entity attachment to -1
        m_FrameBuffer->clearAttachments(1, -1);

        switch(m_SceneState)
        {
            case SceneState::Edit:
            {
                if (m_ViewportFocused)
                {
                    m_EditorCamera.onUpdate(ts);
                }

                m_ActiveScene->onUpdateEditor(ts, m_EditorCamera);
                break;
            }
            case SceneState::Simulate:
            {
                m_EditorCamera.onUpdate(ts);

                m_ActiveScene->onUpdateSimulation(ts, m_EditorCamera);
                break;
            }
            case SceneState::Play:
            {
                m_ActiveScene->onUpdateRuntime(ts);
                break;
            }
        }
    }
    
    {
        FH_PROFILE_SCOPE("Renderer Draw");
        
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

        onOverlayRender();        

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

    ImGui::Begin("Settings");
    ImGui::Checkbox("Show Physics Colliders", &m_ShowPhysicsCollider);
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

    UI_Toolbar();

    ImGui::End();
}

void EditorLayer::UI_Toolbar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    auto& colors = ImGui::GetStyle().Colors;
    auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
    auto& buttonActive = colors[ImGuiCol_ButtonHovered];
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

    ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    float size = ImGui::GetWindowHeight() - 4.0f;

    // Play Button
    {
        Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
        ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));
        if (ImGui::ImageButton("##toolbarPlaybutton", (ImTextureID)icon->getRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0)))
        {
            if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
                onScenePlay();
            else if (m_SceneState == SceneState::Play)
                onSceneStop();
        }
    }
    ImGui::SameLine();

    // Simulate Button
    {
        Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
        if (ImGui::ImageButton("##toolbarSimulatebutton", (ImTextureID)icon->getRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0)))
        {
            if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
                onSceneSimulate();
            else if (m_SceneState == SceneState::Simulate)
                onSceneStop();
        }
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(3);
    ImGui::End();
}

void EditorLayer::onEvent(Event &event)
{
    m_EditorCamera.onEvent(event);

    EventDispatcher dispatcher(event);
    dispatcher.dispatch<KeyPressedEvent>(FH_BIND_EVENT_FN(EditorLayer::onKeyPressed));
    dispatcher.dispatch<MouseButtonPressedEvent>(FH_BIND_EVENT_FN(EditorLayer::onMouseButtonPressed));
}

void EditorLayer::onScenePlay()
{
    if (m_SceneState == SceneState::Simulate)
        onSceneStop();
    
    m_SceneState = SceneState::Play;
    
    m_ActiveScene = Scene::copy(m_Editorscene);
    m_ActiveScene->onRuntimeStart();

    m_SceneHierarchyPanel.setContext(m_ActiveScene);
}

void EditorLayer::onSceneSimulate()
{
    if (m_SceneState == SceneState::Play)
        onSceneStop();

    m_SceneState = SceneState::Simulate;

    m_ActiveScene = Scene::copy(m_Editorscene);
    m_ActiveScene->onSimulationStart();

    m_SceneHierarchyPanel.setContext(m_ActiveScene);    
}

void EditorLayer::onSceneStop()
{
    if (m_SceneState == SceneState::Play)
        m_ActiveScene->onRuntimeStop();
    else if (m_SceneState == SceneState::Simulate)
        m_ActiveScene->onSimulationStop();

    m_SceneState = SceneState::Edit;
    m_ActiveScene = m_Editorscene;

    m_SceneHierarchyPanel.setContext(m_ActiveScene);    
}

void EditorLayer::onDeleteEntity()
{
    if (m_SceneState != SceneState::Edit)
        return;

    Entity selectedEntity = m_SceneHierarchyPanel.getSelectedEntity();
    if (selectedEntity)
    {
        m_Editorscene->destroyEntity(selectedEntity);
        m_SceneHierarchyPanel.setSelectedEntity({});
    }
}

void EditorLayer::onDuplicateEntity()
{
    if (m_SceneState != SceneState::Edit)
        return;

    Entity selectedEntity = m_SceneHierarchyPanel.getSelectedEntity();
    if (selectedEntity)
    {
        m_Editorscene->duplicateEntity(selectedEntity);
    }
}

bool EditorLayer::onKeyPressed(KeyPressedEvent &event)
{
    if(event.getRepeatCount() > 0)
        return false;

    bool controlPressed = Input::isKeyPressed(Key::LeftControl) || Input::isKeyPressed(Key::RightControl);
    bool shiftPressed = Input::isKeyPressed(Key::LeftShift) || Input::isKeyPressed(Key::RightShift);
    switch (event.getKeyCode())
    {
        // File Dialogs
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
            if (controlPressed)
            {
                if (shiftPressed)
                    saveSceneAs();
                else
                    saveScene();
            }
            break;
        }

        // Commands
        case Key::D:
        {   
            if (controlPressed)
                onDuplicateEntity();
            break;
        }

        case Key::Delete:
        {   
            onDeleteEntity();
            break;
        }

        // Gizmos
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

void EditorLayer::onOverlayRender()
{
    if (m_SceneState == SceneState::Play)
    {
        Entity camera = m_ActiveScene->getPrimaryCameraEntity();
        Renderer2D::beginScene(camera.getComponent<CameraComponent>().camera, camera.getComponent<TransformComponent>().getTransform());
    }
    else
    {
        Renderer2D::beginScene(m_EditorCamera);
    }

    if (m_ShowPhysicsCollider)
    {

        
        // Box Collider
        {
            auto view = m_ActiveScene->getAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
            for (auto entity : view)
            {
                auto [transform, boxCollider2D] = view.get<TransformComponent, BoxCollider2DComponent>(entity);
                
                glm::vec3 translation = transform.translation + glm::vec3(boxCollider2D.offset, 0.001f);
                float rotation = transform.rotation.z;
                glm::vec3 scale = transform.scale * glm::vec3(boxCollider2D.size * 2.0f, 1.0f);
                
                glm::mat4 tc = glm::translate(glm::mat4(1.0f), translation)
                * glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f))
                * glm::scale(glm::mat4(1.0f), scale);
                
                Renderer2D::drawRect(tc, glm::vec4(0, 1, 0, 1));
            }   
        }
        
        // Circle Collider
        {
            auto view = m_ActiveScene->getAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
            for (auto entity : view)
            {
                auto [transform, circleCollider2D] = view.get<TransformComponent, CircleCollider2DComponent>(entity);
                
                glm::vec3 translation = transform.translation + glm::vec3(circleCollider2D.offset, 0.001f);
                glm::vec3 scale = transform.scale * glm::vec3(circleCollider2D.radius * 2.0f);
                
                glm::mat4 tc = glm::translate(glm::mat4(1.0f), translation)
                * glm::scale(glm::mat4(1.0f), scale);
                
                Renderer2D::drawCircle(tc, glm::vec4(0, 1, 0, 1), 0.01f);
            }   
        }
    }
       
    Renderer2D::endScene();
}

void EditorLayer::newScene()
{
    m_ActiveScene = createRef<Scene>();
    m_ActiveScene->onViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
    m_SceneHierarchyPanel.setContext(m_ActiveScene);
    m_EditorScenePath = std::filesystem::path();
}

void EditorLayer::saveScene()
{
    if (!m_EditorScenePath.empty())
        serializeScene(m_ActiveScene, m_EditorScenePath);
    else 
        saveSceneAs();
}

void EditorLayer::openScene(const std::filesystem::path& path)
{
    if (m_SceneState != SceneState::Edit)
        onSceneStop();

    Ref<Scene> newScene = createRef<Scene>();
    SceneSerializer serializer(newScene);
    if (serializer.deserialize(path.string()))
    {
        m_Editorscene = newScene;
        m_Editorscene->onViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SceneHierarchyPanel.setContext(m_Editorscene);

        m_ActiveScene = m_Editorscene;
        m_EditorScenePath = path;
    }
   
}

void EditorLayer::openScene()
{
    std::string filePath = FileDialogs::openFile("Fahren Scene (*.fahren)\0*.fahren\0");
    if (!filePath.empty())
    {
        openScene(filePath);
    }
}

void EditorLayer::serializeScene(Ref<Scene> scene, const std::filesystem::path &path)
{
    SceneSerializer serializer(scene);
    serializer.serialize(path);
}

void EditorLayer::saveSceneAs()
{
    std::string filePath = FileDialogs::saveFile("Fahren Scene (*.fahren)\0*.fahren\0");
    if (!filePath.empty())
    {
        serializeScene(m_ActiveScene, filePath);

        m_EditorScenePath = filePath;
    }
}

