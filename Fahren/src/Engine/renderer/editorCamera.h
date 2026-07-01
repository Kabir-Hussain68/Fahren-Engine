#pragma once

#include "camera.h"
#include "Engine/core/timestep.h"
#include "Engine/events/events.h"
#include "Engine/events/mouseEvent.h"

#include <glm/glm.hpp>

class EditorCamera : public Camera
{
private:
    float m_Fov = 45.0f;
    float m_AspectRatio = 1.778f;
    float m_NearClip = 0.1f;
    float m_FarClip = 1000.0f;

    glm::mat4 m_ViewMatrix;
    glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

    glm::vec2 m_InitialMousePosition;

    float m_Distance = 10.0f;
    float m_Pitch = 0.0f;
    float m_Yaw = 0.0f;

    float m_ViewportWidth = 1920;
    float m_ViewportHeight = 1080;

    void updateProjection();
    void updateView();

    bool onMouseScroll(MouseScrolledEvent& event);

    void mousePan(const glm::vec2& delta);
    void mouseRotate(const glm::vec2& delta);
    void mouseZoom(float delta);

    glm::vec3 calculatePosition() const;

    std::pair<float, float> panSpeed() const;
    float rotationSpeed() const;
    float zoomSpeed() const;

public:
    EditorCamera() = default;
    EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

    void onUpdate(Timestep ts);
    void onEvent(Event& event);

    inline float getDistance() const { return m_Distance; }
    inline void setDistance(float distance) { m_Distance = distance; }

    inline void setViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; updateProjection(); }
    
    const glm::mat4& getViewMatrix() const { return m_ViewMatrix; }
    glm::mat4 getViewProjectionMatrix() const { return m_Projection * m_ViewMatrix; }

    glm::vec3 getUpDirection() const;
    glm::vec3 getRightDirection() const;
    glm::vec3 getForwardDirection() const;
    const glm::vec3& getPosition() const { return m_Position; }
    glm::quat getOrientation() const;

    float getPitch() const { return m_Pitch; }
    float getYaw() const { return m_Yaw; }
};