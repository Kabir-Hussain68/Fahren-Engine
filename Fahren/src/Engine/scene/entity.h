#pragma once

#include "Engine/core/UUID.h"
#include "scene.h"
#include "components.h"

#include "entt.hpp"

class Entity
{
private:
    entt::entity m_EntityHandle{ entt::null };
    Scene* m_Scene = nullptr;

public:
    Entity() = default;
    Entity(entt::entity handle, Scene* scene);
    Entity(const Entity& other) = default;

    // For adding a component to the entity
    // It checks if the entity already has the component, if not the component is added 
    template<typename T, typename... Args>
    T& addComponent(Args&&... args)
    {
        FH_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");
        T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        m_Scene->onComponentAdded<T>(*this, component);
        return component;
    }

    template<typename T, typename... Args>
    T& addOrReplaceComponent(Args&&... args)
    {
        T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
        m_Scene->onComponentAdded<T>(*this, component);
        return component;
    }

    // For getting a component from the entity
    // It checks if the entity already has the component, if true, it returns 
    template<typename T>
    T& getComponent()
    {
        FH_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
        return m_Scene->m_Registry.get<T>(m_EntityHandle);
    }

    // For adding a component to the entity
    // It checks if the entity already has the component, it returns true
    template<typename T>
    bool hasComponent()
    {
        return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
    }

    // For removing a component from the entity
    // It checks if the entity already has the component, if true the component is removed 
    template<typename T>
    void removeComponent()
    {
        FH_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
        m_Scene->m_Registry.remove<T>(m_EntityHandle);
    }

    // Helper functions for comparing entities
    // entt::entt (Entity) is just a uint32_t
    operator bool() const { return m_EntityHandle != entt::null; }
    operator entt::entity() const { return m_EntityHandle; }
    operator uint32_t() const { return (uint32_t)m_EntityHandle; }

    UUID getUUID() { return getComponent<IDComponent>().id; }
    const std::string& getName() { return getComponent<TagComponent>().tag; }

    bool operator==(const Entity& other) const 
    { 
        return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; 
    }

    bool operator!=(const Entity& other) const
    {
        return !operator==(other);
    }
};