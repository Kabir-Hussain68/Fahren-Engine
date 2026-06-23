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
    Camera* mainCamera = nullptr;
    glm::mat4* cameraTransform = nullptr;
    {
        auto view = m_Registry.view<TransformComponent, CameraComponent>();
        for (auto entity : view)
        {
            auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
            if (camera.primary)
            {
                mainCamera = &camera.camera;
                cameraTransform = &transform.transform;
                break;
            }    
        }
    }

    if (mainCamera)
    {
        Renderer2D::beginScene(*mainCamera, *cameraTransform);

        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group)
        {
            auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
    
            Renderer2D::drawQuad(transform, sprite.color);
        }

        Renderer2D::endScene();
    }

}

void Scene::onViewportResize(uint32_t width, uint32_t height)
{
    m_ViewportWidth = width;
    m_ViewportHeight = height;

    auto view = m_Registry.view<CameraComponent>();
    for (auto entity : view)
    {
        auto& cameraComponent = view.get<CameraComponent>(entity);
        if (!cameraComponent.fixedAspectRatio)
        {
            cameraComponent.camera.setViewportSize(width, height);
        }
    }
}
