#include "fhpch.h"
#include "scene.h"

#include "components.h"
#include "Engine/renderer/renderer2D.h"

#include <glm/glm.hpp>

#include "entity.h"

// Physics
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>

// Audio
#include <miniaudio.h>

static b2BodyType rigidBody2DTypeToBox2DType(RigidBody2DComponent::BodyType bodyType)
{
    switch(bodyType)
    {
        case RigidBody2DComponent::BodyType::Static:    return b2_staticBody;
        case RigidBody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
        case RigidBody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
    }

    FH_CORE_ASSERT(false, "Unkown body type!");
    return b2_staticBody;
}

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

void Scene::destroyEntity(Entity entity)
{
    m_Registry.destroy(entity);
}

void Scene::onRuntimeStart()
{
    // Physics
    {

        m_Box2DWorld = new b2World({0.0f, -9.8f});
        auto view = m_Registry.view<RigidBody2DComponent>();
        for (auto e : view)
        {
            Entity entity = {e, this};
            auto& transform = entity.getComponent<TransformComponent>();
            auto& rb2d = entity.getComponent<RigidBody2DComponent>();
            
            b2BodyDef bodyDef;
            bodyDef.type = rigidBody2DTypeToBox2DType(rb2d.type);
            bodyDef.position.Set(transform.translation.x, transform.translation.y);
            bodyDef.angle = transform.rotation.z;
            
            b2Body* body = m_Box2DWorld->CreateBody(&bodyDef);
            body->SetFixedRotation(rb2d.fixedRotation);
            rb2d.Runtime = body;
            
            if (entity.hasComponent<BoxCollider2DComponent>())
            {
                auto& bc2d = entity.getComponent<BoxCollider2DComponent>();
                
                b2PolygonShape boxShape;
                boxShape.SetAsBox(bc2d.size.x * transform.scale.x, bc2d.size.y * transform.scale.y);
                
                b2FixtureDef fixtureDef;
                fixtureDef.shape = &boxShape;
                fixtureDef.density = bc2d.density;
                fixtureDef.friction= bc2d.friction;
                fixtureDef.restitution = bc2d.restitution;
                fixtureDef.restitutionThreshold = bc2d.restitutionThreshold;
                body->CreateFixture(&fixtureDef);
            }
        }
    }

    // Audio
    {
        m_AudioEngine = new ma_engine();
        ma_result audioEngineResult = ma_engine_init(nullptr, m_AudioEngine);
        FH_CORE_ASSERT(audioEngineResult == MA_SUCCESS, "Failed to initialize audio engine");

        auto view = m_Registry.view<AudioSourceComponent>();
        for (auto e : view)
        {
            Entity entity = {e, this};
            auto& audio = entity.getComponent<AudioSourceComponent>();

            ma_sound* sound = new ma_sound();
            ma_result result = ma_sound_init_from_file(m_AudioEngine, audio.audioPath.c_str(), 0, nullptr, nullptr, sound);

            if (result != MA_SUCCESS)
            {
                FH_CORE_ERROR("Failed to load audio file: {0}", audio.audioPath);
                delete sound;
                audio.Runtime = nullptr;
                continue;
            }

            ma_sound_set_volume(sound, audio.volume);
            ma_sound_set_looping(sound, audio.loop);
            audio.Runtime = sound;

            if (audio.playOnStart)
                ma_sound_start(sound);
            }
    }
}

void Scene::onRuntimeStop()
{
    // Physics
    {
        delete m_Box2DWorld;
        m_Box2DWorld = nullptr;
    }

    // Audio
    {
        auto view = m_Registry.view<AudioSourceComponent>();
        for (auto e : view)
        {
            Entity entity = Entity{e, this};
            auto& audio = entity.getComponent<AudioSourceComponent>();
            
            if (audio.Runtime)
            {
                ma_sound* sound = (ma_sound*)audio.Runtime;
                ma_sound_uninit(sound);
                delete sound;
                audio.Runtime = nullptr;
            }
        }
        ma_engine_uninit(m_AudioEngine);

        delete m_AudioEngine;
        m_AudioEngine = nullptr;
    }
}

void Scene::onUpdateEditor(Timestep ts, EditorCamera &camera)
{
    Renderer2D::beginScene(camera);

        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group)
        {
            auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
    
            Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
        }

    Renderer2D::endScene();
}

void Scene::onUpdateRuntime(Timestep ts)
{
    //Update Native Scripts
    {
        m_Registry.view<NativeScriptComponent>().each([this, ts](auto entity, auto& nsc)
        {
            if (!nsc.instance)
            {
                nsc.instance = nsc.instantiateScript();
                nsc.instance->m_Entity = Entity{ entity, this };
                nsc.instance->onCreate();
            }

            nsc.instance->onUpdate(ts);
        });
    }

    // Physics
    {
        const int32_t velocityIterations = 6;
        const int32_t positionIterations = 2;
        m_Box2DWorld->Step(ts, velocityIterations, positionIterations);
        
        // Retrieve transform from Box2D
        auto view = m_Registry.view<RigidBody2DComponent>();
        for (auto e : view)
        {
            Entity entity = {e, this};
            auto& transform = entity.getComponent<TransformComponent>();
            auto& rb2d = entity.getComponent<RigidBody2DComponent>();

            b2Body* body = (b2Body*)rb2d.Runtime;
            const auto& position = body->GetPosition();
            transform.translation.x = position.x;
            transform.translation.y = position.y;
            transform.rotation.z = body->GetAngle();
        }
    }

    // Audio
    {
        auto view = m_Registry.view<AudioSourceComponent>();
        for (auto e : view)
        {
            Entity entity = Entity{e, this};
            auto& audio = entity.getComponent<AudioSourceComponent>();

            if (!audio.Runtime)
                continue;

            ma_sound* sound = static_cast<ma_sound*>(audio.Runtime);

            ma_sound_set_volume(sound, audio.volume);
            ma_sound_set_looping(sound, audio.loop);
        }
    }

    // Render2D
    Camera* mainCamera = nullptr;
    glm::mat4 cameraTransform;
    {
        auto view = m_Registry.view<TransformComponent, CameraComponent>();
        for (auto entity : view)
        {
            auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
            if (camera.primary)
            {
                mainCamera = &camera.camera;
                cameraTransform = transform.getTransform();
                break;
            }    
        }
    }

    if (mainCamera)
    {
        Renderer2D::beginScene(*mainCamera, cameraTransform);

        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group)
        {
            auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
    
            Renderer2D::drawSprite(transform.getTransform(), sprite, (int)entity);
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

Entity Scene::getPrimaryCameraEntity()
{
    auto view = m_Registry.view<CameraComponent>();
    for (auto entity : view)
    {
        const auto& camera = view.get<CameraComponent>(entity);
        if (camera.primary)
            return Entity(entity, this);
    }
    return {};
}

template<typename T>
void Scene::onComponentAdded(Entity entity, T& component)
{
    static_assert(false);

}

template<>
void Scene::onComponentAdded<TagComponent>(Entity entity, TagComponent& component)
{
}

template<>
void Scene::onComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
{
}

template<>
void Scene::onComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
{
    if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
    {
        component.camera.setViewportSize(m_ViewportWidth, m_ViewportHeight);
    }
}

template<>
void Scene::onComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
{
}

template<>
void Scene::onComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
{
}

template<>
void Scene::onComponentAdded<RigidBody2DComponent>(Entity entity, RigidBody2DComponent& component)
{
}

template<>
void Scene::onComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
{
}

template<>
void Scene::onComponentAdded<AudioSourceComponent>(Entity entity, AudioSourceComponent& component)
{
}
