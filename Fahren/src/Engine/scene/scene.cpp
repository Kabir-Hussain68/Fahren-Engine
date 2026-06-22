#include "fhpch.h"
#include "scene.h"

#include "components.h"
#include "Engine/renderer/renderer2D.h"

#include <glm/glm.hpp>

#include "entity.h"

Scene::Scene()
{

    
}

Scene::~Scene()
{
}

Entity Scene::createEntity(const std::string& name)
{
    Entity entity = { m_Registry.create(), this };
    entity.addComponent<TransformComponent>();
    auto& tag = entity.addComponent<TagComponent>();
    tag.tag = name.empty() ? "Entity" : name;
    return entity;
}

void Scene::onUpdate(Timestep ts)
{
    auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
    for (auto entity : group)
    {
        auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

        Renderer2D::drawQuad(transform, sprite.color);
    }
}
