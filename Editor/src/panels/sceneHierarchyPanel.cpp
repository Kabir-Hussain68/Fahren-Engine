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
}
