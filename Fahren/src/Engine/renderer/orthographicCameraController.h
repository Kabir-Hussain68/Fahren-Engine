#pragma once

#include "Engine/core/timestep.h"
#include "orthographicCamera.h"

#include "Engine/events/applicationEvent.h"
#include "Engine/events/mouseEvent.h"

class OrthographicCameraController
{
private:
    float m_AspectRatio;
    float m_ZoomLevel = 1.0f;
    OrthographicCamera m_Camera;

    bool m_Rotation;

    glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
    float m_CameraRotation = 0.0f;

    float m_CameraTranslationSpeed = 5.0f;
    float m_CameraRotationSpeed = 180.0f;

    bool onMouseScrolled(MouseScrolledEvent& event);
    bool onWindowResized(WindowResizeEvent& event);

public:
    OrthographicCameraController(float aspectRatio, bool rotaion = false);

    void onUpdate(Timestep ts);
    void onEvent(Event& event);

    OrthographicCamera& getCamera() { return m_Camera; }
    const OrthographicCamera& getCamera() const { return m_Camera; }
};