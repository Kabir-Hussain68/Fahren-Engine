#pragma once

#include "Engine/renderer/camera.h"

class SceneCamera : public Camera
{
public :
    enum class ProjectionType { Prespective = 0, Orthographic = 1 };

private:
    ProjectionType m_ProjectionType = ProjectionType::Orthographic;

    float m_OrthographicSize = 10.0f;
    float m_OrthographicNear = -1.0f;
    float m_OrthographicFar = 1.0f;

    float m_PrespectiveFOV = glm::radians(45.0f);
    float m_PrespectiveNear = 0.01f;
    float m_PrespectiveFar = 1000.0f;

    float m_AspectRatio = 0.0f;

    void recalculateProjection();

public:
    SceneCamera();
    virtual ~SceneCamera() = default;

    void setOrthographic(float size, float nearClip, float farClip);
    void setPrespective(float fov, float nearClip, float farClip);

    void setViewportSize(uint32_t width, uint32_t height);

    float getPrespectiveVerticalFOV() const { return m_PrespectiveFOV; }
    void setPrespectiveVerticalFOV(float fov) { m_PrespectiveFOV = fov; recalculateProjection(); }
    float getPrespectiveNearClip() const { return m_PrespectiveNear; }
    void setPrespectiveNearClip(float nearClip) { m_PrespectiveNear = nearClip; recalculateProjection(); }
    float getPrespectiveFarClip() const { return m_PrespectiveFar; }
    void setPrespectiveFarClip(float farClip) { m_PrespectiveFar = farClip; recalculateProjection(); }

    float getOrthographicSize() const { return m_OrthographicSize; }
    void setOrthographicSize(float size) { m_OrthographicSize = size; recalculateProjection(); }
    float getOrthographicNearClip() const { return m_OrthographicNear; }
    void setOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; recalculateProjection(); }
    float getOrthographicFarClip() const { return m_OrthographicFar; }
    void setOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; recalculateProjection(); }

    ProjectionType getProjectionType() const { return m_ProjectionType; }
    void setProjectionType(ProjectionType type) { m_ProjectionType = type; recalculateProjection(); }
};