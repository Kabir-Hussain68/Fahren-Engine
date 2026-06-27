#include "sceneHierarchyPanel.h"

#include "Engine/scene/components.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> &context)
{
    setContext(context);
}

void SceneHierarchyPanel::setContext(const Ref<Scene> &context)
{
    m_Context = context;
}

void SceneHierarchyPanel::onImGuiRender()
{
    ImGui::Begin("Scene Hierarchy");

    m_Context->m_Registry.view<entt::entity>().each([this](auto entityID)
    {
        Entity entity{ entityID, m_Context.get() };
        drawEntityNode(entity);
    });

    if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
    {
        m_SelectionContext = {};
    }

    ImGui::End();

    ImGui::Begin("Properties Panel");
    if (m_SelectionContext)
    {
        drawComponents(m_SelectionContext);
    }

    ImGui::End();
}

void SceneHierarchyPanel::drawEntityNode(Entity entity)
{
    auto& tag =  entity.getComponent<TagComponent>().tag;

    ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", tag.c_str());
    if (ImGui::IsItemClicked())
    {
        m_SelectionContext = entity;
    }

    if (opened)
    {
        ImGui::TreePop();
    }

}

void SceneHierarchyPanel::drawComponents(Entity entity)
{
    if (entity.hasComponent<TagComponent>())
    {
        auto& tag =  entity.getComponent<TagComponent>().tag;

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strcpy(buffer, tag.c_str());
        if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
        {
            tag = std::string(buffer);
        }
    }

    if (entity.hasComponent<TransformComponent>())
    {
        if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
        {
            auto& transform = entity.getComponent<TransformComponent>().transform;
            ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);

            ImGui::TreePop();
        }
    }

    if (entity.hasComponent<CameraComponent>())
    {
        if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
        {
            auto& cameraComponent = entity.getComponent<CameraComponent>();
            auto& camera = cameraComponent.camera;

            ImGui::Checkbox("Primary", &cameraComponent.primary);

            const char* projectionTypeStrings[] = {"Prespective", "Orthographic"};
            const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.getProjectionType()];

            if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
            {
                for (int i = 0; i < 2; i++)
                {
                    bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
                    if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
                    {
                        currentProjectionTypeString = projectionTypeStrings[i];
                        camera.setProjectionType((SceneCamera::ProjectionType)i);
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            if (camera.getProjectionType() ==  SceneCamera::ProjectionType::Prespective)
            {
                float prespectiveFov = glm::degrees(camera.getPrespectiveVerticalFOV());
                if (ImGui::DragFloat("FOV", &prespectiveFov))
                {
                    camera.setPrespectiveVerticalFOV(glm::radians(prespectiveFov));
                }
                
                float prespectiveNear = camera.getPrespectiveNearClip();
                if (ImGui::DragFloat("NearClip", &prespectiveNear))
                {
                    camera.setPrespectiveNearClip(prespectiveNear);
                }

                float prespectiveFar = camera.getPrespectiveFarClip();
                if (ImGui::DragFloat("FarClip", &prespectiveFar))
                {
                    camera.setPrespectiveFarClip(prespectiveFar);
                }
            }

            if (camera.getProjectionType() ==  SceneCamera::ProjectionType::Orthographic)
            {
                float orthoSize = camera.getOrthographicSize();
                if (ImGui::DragFloat("Size", &orthoSize))
                {
                    camera.setOrthographicSize(orthoSize);
                }
                
                float orthoNear = camera.getOrthographicNearClip();
                if (ImGui::DragFloat("NearClip", &orthoNear))
                {
                    camera.setOrthographicNearClip(orthoNear);
                }

                float orthoFar = camera.getOrthographicFarClip();
                if (ImGui::DragFloat("FarClip", &orthoFar))
                {
                    camera.setOrthographicFarClip(orthoFar);
                }

                ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.fixedAspectRatio);
            }

            ImGui::TreePop();
        }
    }

    if (entity.hasComponent<SpriteRendererComponent>())
    {
        if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
        {
            auto& src = entity.getComponent<SpriteRendererComponent>();
            ImGui::ColorEdit4("Color", glm::value_ptr(src.color));

            ImGui::TreePop();
        }
    }
}
