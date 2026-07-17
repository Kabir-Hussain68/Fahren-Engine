#pragma once

#include "entt.hpp"

#include "Engine/core/timestep.h"
#include "Engine/core/UUID.h"
#include "Engine/renderer/editorCamera.h"

class b2World;
struct ma_engine;

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

    b2World* m_Box2DWorld = nullptr;

    ma_engine* m_AudioEngine = nullptr;

    template<typename T>
    void onComponentAdded(Entity entity, T& component);

    void onPhysicsStart();
    void onPhysicsStop();

    void onRenderScene(EditorCamera& camera);
public:
    Scene();
    ~Scene();

    static Ref<Scene> copy(Ref<Scene> src);

    Entity createEntity(const std::string& name = std::string());
    Entity createEntityWithUUID(UUID uuid, const std::string& name = std::string());
    void destroyEntity(Entity entity);

    void duplicateEntity(Entity entity);

    void onRuntimeStart();
    void onRuntimeStop();

    void onSimulationStart();
    void onSimulationStop();

    void onUpdateEditor(Timestep ts, EditorCamera& camera);
    void onUpdateRuntime(Timestep ts);
    void onUpdateSimulation(Timestep ts, EditorCamera& camera);

    void onViewportResize(uint32_t width, uint32_t height);

    Entity getPrimaryCameraEntity();

    template<typename... Components>
    auto getAllEntitiesWith()
    {
        return m_Registry.view<Components...>();
    }
};