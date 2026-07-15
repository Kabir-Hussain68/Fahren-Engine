#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "sceneCamera.h"
#include "Engine/renderer/texture.h"
#include "Engine/core/UUID.h"

// Components which the entities can be assigned

struct IDComponent
{
    UUID id;

    IDComponent() = default;
    IDComponent(const IDComponent&) = default;
    IDComponent(const UUID& uuid)
        : id(uuid)  {}
};

struct TagComponent
{
    std::string tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag)
        : tag(tag)  {}
};

struct TransformComponent
{
    glm::vec3 translation = {0.0f, 0.0f, 0.0f};
    glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 scale = {1.0f, 1.0f, 1.0f};

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3 translation)
        : translation(translation) {}

    glm::mat4 getTransform() const
    {
        glm::mat4 rotate = glm::toMat4(glm::quat(rotation));

        return glm::translate(glm::mat4(1.0f), translation)
                              * rotate
                              * glm::scale(glm::mat4(1.0f), scale);
    }
};

struct SpriteRendererComponent
{
    glm::vec4 color = glm::vec4(1.0f);
    Ref<Texture2D> texture;
    float tilingFactor = 1.0f;

    SpriteRendererComponent() = default;
    SpriteRendererComponent(const SpriteRendererComponent&) = default;
    SpriteRendererComponent(const glm::vec4 color)
        : color(color) {}

};

struct CameraComponent
{
    SceneCamera camera;
    bool primary = true;
    bool fixedAspectRatio = false;

    CameraComponent() = default;
    CameraComponent(const CameraComponent&) = default;
};

class ScriptableEntity;
struct NativeScriptComponent
{
    ScriptableEntity* instance = nullptr;

    ScriptableEntity*(*instantiateScript)();
    void (*destroyScript)(NativeScriptComponent*);

    template<typename T>
    void bind()
    {
        instantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
        destroyScript = [](NativeScriptComponent* nsc) { delete nsc->instance; nsc->instance = nullptr; };
    }
};

struct RigidBody2DComponent
{
    enum class  BodyType { Static = 0, Dynamic, Kinematic };
    BodyType type  = BodyType::Static;
    bool fixedRotation = false;

    //Storage for runtime
    void* Runtime = nullptr;

    RigidBody2DComponent() = default;
    RigidBody2DComponent(const RigidBody2DComponent&) = default;
};

struct BoxCollider2DComponent
{
    glm::vec2 offset = {0.0f, 0.0f};
    glm::vec2 size = {0.5f, 0.5f};

    float density = 1.0f;
    float friction = 0.5f;
    float restitution = 0.0f;
    float restitutionThreshold = 0.5f;

    //Storage for runtime
    void* RuntimeFixture = nullptr;

    BoxCollider2DComponent() = default;
    BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
};

struct AudioSourceComponent
{
    std::string audioPath;
    float volume = 1.0f;
    bool loop = false;
    bool playOnStart = false;

    //Storage for runtime
    void* Runtime = nullptr;

    AudioSourceComponent() = default;
    AudioSourceComponent(const AudioSourceComponent&) = default;
};