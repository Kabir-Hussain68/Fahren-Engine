#include "sceneHierarchyPanel.h"

#include "Engine/scene/components.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

extern const std::filesystem::path g_AssetPath;

static Ref<Texture2D> getSettingsIcon()
{
    static Ref<Texture2D> settingsIcon = Texture2D::create("resources/icons/properties/settings.png");
    return settingsIcon;
}

static void drawFullWidthSeparator()
{
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 p1 = ImGui::GetCursorScreenPos();
    float width = ImGui::GetContentRegionAvail().x;
    ImVec2 p2 = ImVec2(p1.x + width, p1.y);
    drawList->AddLine(p1, p2, ImGui::GetColorU32(ImGuiCol_Separator));
    ImGui::Dummy(ImVec2(0.0f, 2.0f));
}

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

void SceneHierarchyPanel::setSelectedEntity(Entity entity)
{
    m_SelectionContext = entity;
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
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened = ImGui::TreeNodeEx((void*)9817239, flags, "%s", tag.c_str());
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

    ImGui::Columns(2, nullptr, false);
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

    drawFullWidthSeparator();
}

static void drawTextureField(const std::string& label, Ref<Texture2D>& texture, const std::filesystem::path& assetPath, float columnWidth = 100.0f)
{
    ImGui::PushID(label.c_str());

    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    float thumbnailSize = 40.0f;

    if (texture)
    {
        ImGui::ImageButton("##texture", (ImTextureID)texture->getRendererID(),
            { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
    }
    else
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
        ImGui::Button("##texture_empty", { thumbnailSize, thumbnailSize });
        ImGui::PopStyleColor(3);
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload("Content_Browser_Item"))
        {
            const char* path = (const char*)payLoad->Data;
            std::filesystem::path texturePath = assetPath / path;
            texture = Texture2D::create(texturePath.string());
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::BeginPopupContextItem("##texture_context"))
    {
        if (ImGui::MenuItem("Clear"))
            texture = nullptr;
        ImGui::EndPopup();
    }

    ImGui::Columns(1);
    ImGui::PopID();

    drawFullWidthSeparator();
}

static void drawFloatField(const std::string& label, float& value, float speed, float minVal, float maxVal, float columnWidth = 100.0f)
{
    ImGui::PushID(label.c_str());

    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::DragFloat("##Value", &value, speed, minVal, maxVal);

    ImGui::Columns(1);
    ImGui::PopID();

    drawFullWidthSeparator();
}

static void drawColorField(const std::string& label, glm::vec4& color, float columnWidth = 100.0f)
{
    ImGui::PushID(label.c_str());

    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::ColorEdit4("##Color", glm::value_ptr(color));

    ImGui::Columns(1);
    ImGui::PopID();

    drawFullWidthSeparator();
}

static void drawAudioField(const std::string& label, std::string& audioPath, const std::filesystem::path& assetPath, float columnWidth = 100.0f)
{
    ImGui::PushID(label.c_str());

    ImGui::Columns(2, nullptr, false);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    std::string displayName = audioPath.empty() ? "None" : std::filesystem::path(audioPath).filename().string();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    ImGui::Button(displayName.c_str(), ImVec2(-1.0f, 0.0f));
    ImGui::PopStyleColor(3);

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload("Content_Browser_Item"))
        {
            const char* path = (const char*)payLoad->Data;
            std::filesystem::path fullPath = assetPath / path;
            audioPath = fullPath.string();
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::BeginPopupContextItem("##audio_context"))
    {
        if (ImGui::MenuItem("Clear"))
            audioPath.clear();
        ImGui::EndPopup();
    }

    ImGui::Columns(1);
    ImGui::PopID();

    drawFullWidthSeparator();
}

template<typename T, typename UIFunction>
static void drawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
{
    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;
    if (entity.hasComponent<T>())
    {
        auto& component = entity.getComponent<T>();
        ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

        ImGui::Dummy(ImVec2(0.0f, 4.0f));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
        float lineHeight = ImGui::GetFontSize() + GImGui->Style.FramePadding.y * 2.0f;

        bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name.c_str());
        ImGui::PopStyleVar();

        float iconSize = 14.0f;
        ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f - 4.0f);

        float framePaddingY = (lineHeight - iconSize) * 0.5f;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, framePaddingY));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.1f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.15f));
        if (ImGui::ImageButton("##settings", (ImTextureID)getSettingsIcon()->getRendererID(),
            ImVec2(iconSize, iconSize), { 0, 1 }, { 1, 0 }))
        {
            ImGui::OpenPopup("ComponentSettings");
        }
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar();

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
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 4.0f));
            uiFunction(component);
            ImGui::PopStyleVar();

            ImGui::TreePop();
        }

        ImGui::Dummy(ImVec2(0.0f, 4.0f));

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

    if (ImGui::Button("ADD"))
    {
        ImGui::OpenPopup("AddComponent");
    }

    if (ImGui::BeginPopup("AddComponent"))
    {
        if (!m_SelectionContext.hasComponent<CameraComponent>())
        {
            if (ImGui::MenuItem("Camera"))
            {
				m_SelectionContext.addComponent<CameraComponent>();
                ImGui::CloseCurrentPopup();
            }
        }

        if (!m_SelectionContext.hasComponent<SpriteRendererComponent>())
        {
            if (ImGui::MenuItem("Sprite Renderer"))
            {
                m_SelectionContext.addComponent<SpriteRendererComponent>();
                ImGui::CloseCurrentPopup();
            }
        }

        if (!m_SelectionContext.hasComponent<RigidBody2DComponent>())
        {
            if (ImGui::MenuItem("Rigid Body"))
            {
                m_SelectionContext.addComponent<RigidBody2DComponent>();
                ImGui::CloseCurrentPopup();
            }
        }

        if (!m_SelectionContext.hasComponent<BoxCollider2DComponent>())
        {
            if (ImGui::MenuItem("Box Collider"))
            {
                m_SelectionContext.addComponent<BoxCollider2DComponent>();
                ImGui::CloseCurrentPopup();
            }
        }

        if (!m_SelectionContext.hasComponent<AudioSourceComponent>())
        {
            if (ImGui::MenuItem("Audio Source"))
            {
                m_SelectionContext.addComponent<AudioSourceComponent>();
                ImGui::CloseCurrentPopup();
            }
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
        drawFullWidthSeparator();

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
        drawFullWidthSeparator();

        if (camera.getProjectionType() ==  SceneCamera::ProjectionType::Prespective)
        {
            float prespectiveFov = glm::degrees(camera.getPrespectiveVerticalFOV());
            if (ImGui::DragFloat("FOV", &prespectiveFov))
            {
                camera.setPrespectiveVerticalFOV(glm::radians(prespectiveFov));
            }
            drawFullWidthSeparator();

            float prespectiveNear = camera.getPrespectiveNearClip();
            if (ImGui::DragFloat("NearClip", &prespectiveNear))
            {
                camera.setPrespectiveNearClip(prespectiveNear);
            }
            drawFullWidthSeparator();

            float prespectiveFar = camera.getPrespectiveFarClip();
            if (ImGui::DragFloat("FarClip", &prespectiveFar))
            {
                camera.setPrespectiveFarClip(prespectiveFar);
            }
            drawFullWidthSeparator();
        }

        if (camera.getProjectionType() ==  SceneCamera::ProjectionType::Orthographic)
        {
            float orthoSize = camera.getOrthographicSize();
            if (ImGui::DragFloat("Size", &orthoSize))
            {
                camera.setOrthographicSize(orthoSize);
            }
            drawFullWidthSeparator();

            float orthoNear = camera.getOrthographicNearClip();
            if (ImGui::DragFloat("NearClip", &orthoNear))
            {
                camera.setOrthographicNearClip(orthoNear);
            }
            drawFullWidthSeparator();

            float orthoFar = camera.getOrthographicFarClip();
            if (ImGui::DragFloat("FarClip", &orthoFar))
            {
                camera.setOrthographicFarClip(orthoFar);
            }
            drawFullWidthSeparator();

            ImGui::Checkbox("Fixed Aspect Ratio", &component.fixedAspectRatio);
            drawFullWidthSeparator();
        }
    });

    drawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
    {
        drawColorField("Color", component.color);
        drawTextureField("Texture", component.texture, g_AssetPath);
        drawFloatField("Tiling Factor", component.tilingFactor, 0.1f, 1.0f, 10.0f);
    });

    drawComponent<RigidBody2DComponent>("Rigid Body", entity, [](auto& component)
    {
        const char* bodyTypeStrings[] = {"Static", "Dynamic", "Kinematic"};
        const char* currentBodyTypeString = bodyTypeStrings[(int)component.type];
        if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
        {
            for (int i = 0; i < 2; i++)
            {
                bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
                if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
                {
                    currentBodyTypeString = bodyTypeStrings[i];
                    component.type = ((RigidBody2DComponent::BodyType)i);
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        ImGui::Checkbox("Fixed Rotation", &component.fixedRotation);
    });

    drawComponent<BoxCollider2DComponent>("Box Collider", entity, [](auto& component)
    {
        ImGui::DragFloat2("Offset", glm::value_ptr(component.offset));
        ImGui::DragFloat2("Size", glm::value_ptr(component.size));
        drawFloatField("Density", component.density, 0.01f, 0.0f, 1.0f);
        drawFloatField("Friction", component.friction, 0.01f, 0.0f, 1.0f);
        drawFloatField("Restitution", component.restitution, 0.1f, 0.0f, 1.0f);
        drawFloatField("RestitutionThreshold", component.restitutionThreshold, 0.01f, 1.0f, 10.0f);
    });

    drawComponent<AudioSourceComponent>("Audio Source", entity, [](auto& component)
    {
        drawAudioField("Audio Clip", component.audioPath, g_AssetPath);
        drawFloatField("Volume", component.volume, 0.01f, 0.0f, 1.0f);

        ImGui::Checkbox("Loop", &component.loop);
        drawFullWidthSeparator();

        ImGui::Checkbox("Play On Start", &component.playOnStart);
        drawFullWidthSeparator();
    });
}