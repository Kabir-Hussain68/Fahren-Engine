#pragma once

#include "entt.hpp"

#include "Engine/core/timestep.h"

class Entity;

class Scene
{
private:
    friend class Entity;
    friend class SceneSerializer;
    friend class SceneHierarchyPanel;

    entt::registry m_Registry;

    uint32_t m_ViewportWidth = 0;
    uint32_t m_ViewportHeight = 0;

    template<typename T>
    void onComponentAdded(Entity entity, T& component);

public:
    Scene();
    ~Scene();

    Entity createEntity(const std::string& name = std::string());
    void destroyEntity(Entity entity);

    void onUpdate(Timestep ts);
    void onViewportResize(uint32_t width, uint32_t height);

    Entity getPrimaryCameraEntity();
};