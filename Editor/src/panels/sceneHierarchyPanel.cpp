#include "sceneHierarchyPanel.h"

#include "Engine/scene/components.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene> &context)
{
    setContext(context);
}

void SceneHierarchyPanel::setContext(const Ref<Scene> &context)
{
    m_Context = context;
    m_SelectionContext = {};
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

    //Right Click blank space
    if (ImGui::BeginPopupContextWindow("WindowContextMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
    {
        if (ImGui::MenuItem("Empty Entity"))
        {
            m_Context->createEntity("Empty Entity");
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
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
    flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", tag.c_str());
    if (ImGui::IsItemClicked())
    {
        m_SelectionContext = entity;
    }

    bool entityDeleted = false;
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Delete Entity"))
        {
            entityDeleted = true;
        }

        ImGui::EndPopup();
    }

    if (opened)
    {
        //Havent implemented drop down menu yet
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened = ImGui::TreeNodeEx((void*)9817239, flags, "%s", tag.c_str());  // <-- second TreeNodeEx call
        if (opened)
            ImGui::TreePop();
        ImGui::TreePop();
    }

    if (entityDeleted)
    {
        m_Context->destroyEntity(entity);
        if (m_SelectionContext == entity)
        {
            m_SelectionContext = {};
        }
    }
}

static void drawVec3Controls(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.0f));

    ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize))
        values.x = resetValue;
    
    ImGui::PopFont();
    ImGui::PopStyleColor(3);
    
    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.0f));

    ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize))
        values.y = resetValue;
    
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.0f));

    ImGui::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize))
        values.z = resetValue;
    
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();
}

template<typename T, typename UIFunction>
static void drawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
{
    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;
    if (entity.hasComponent<T>())
    {
        auto& component = entity.getComponent<T>();
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
        float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;
        ImGui::Separator();

        bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name.c_str());
        ImGui::PopStyleVar();

        ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
        if (ImGui::Button("+", ImVec2(lineHeight, lineHeight)))
        {
            ImGui::OpenPopup("ComponentSettings");
        }

        bool removeComponent = false;
        if (ImGui::BeginPopup("ComponentSettings"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                removeComponent = true;
            }

            ImGui::EndPopup();
        }

        if (open)
        {
            uiFunction(component);

            ImGui::TreePop();
        }

        if (removeComponent)
        {
            entity.removeComponent<T>();
        }
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
        if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
        {
            tag = std::string(buffer);
        }
    }

    ImGui::SameLine();
    ImGui::PushItemWidth(-1);

    if (ImGui::Button("Add Component"))
    {
        ImGui::OpenPopup("AddComponent");
    }

    if (ImGui::BeginPopup("AddComponent"))
    {
        if (ImGui::MenuItem("Camera"))
        {
            if (!m_SelectionContext.hasComponent<CameraComponent>())
				m_SelectionContext.addComponent<CameraComponent>();
			else
				FH_CORE_WARN("This entity already has the Camera Component!");
			ImGui::CloseCurrentPopup();
        }

        if (ImGui::MenuItem("Sprite Renderer"))
        {
            if (!m_SelectionContext.hasComponent<SpriteRendererComponent>())
				m_SelectionContext.addComponent<SpriteRendererComponent>();
			else
				FH_CORE_WARN("This entity already has the Sprite Renderer Component!");
			ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::PopItemWidth();

    drawComponent<TransformComponent>("Transform", entity, [](auto& component)
    {
        drawVec3Controls("Position", component.translation);
        glm::vec3 rotate = glm::degrees(component.rotation);
        drawVec3Controls("Rotation", rotate);
        component.rotation = glm::radians(rotate);
        drawVec3Controls("Scale", component.scale, 1.0f);
    });

    drawComponent<CameraComponent>("Camera", entity, [](auto& component)
    {
        auto& camera = component.camera;

        ImGui::Checkbox("Primary", &component.primary);

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

            ImGui::Checkbox("Fixed Aspect Ratio", &component.fixedAspectRatio);
        }
    });

    drawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
    {
        ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
    });
}
