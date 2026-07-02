#pragma once

#include "Engine/core/base.h"
#include "Engine/scene/scene.h"
#include "Engine/scene/entity.h"

class SceneHierarchyPanel
{
private:
    Ref<Scene> m_Context;
    Entity m_SelectionContext;

    void drawEntityNode(Entity entity);
    void drawComponents(Entity entity);

public:
    SceneHierarchyPanel() = default;
    SceneHierarchyPanel(const Ref<Scene>& context);

    void setContext(const Ref<Scene>& context);

    void onImGuiRender();

    Entity getSelectedEntity()const { return m_SelectionContext; }
    void setSelectedEntity(Entity entity);
};