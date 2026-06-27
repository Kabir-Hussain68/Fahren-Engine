#include "fhpch.h"
#include "sceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

SceneCamera::SceneCamera()
{
    recalculateProjection();
}

void SceneCamera::setOrthographic(float size, float nearClip, float farClip)
{
    m_ProjectionType = ProjectionType::Orthographic;
    m_OrthographicSize = size;
    m_OrthographicNear = nearClip;
    m_OrthographicFar = farClip;

    recalculateProjection();
}

void SceneCamera::setPrespective(float fov, float nearClip, float farClip)
{
    m_ProjectionType = ProjectionType::Prespective;
    m_PrespectiveFar = fov;
    m_PrespectiveNear = nearClip;
    m_PrespectiveFar = farClip;
}

void SceneCamera::setViewportSize(uint32_t width, uint32_t height)
{
    m_AspectRatio = (float)width / (float)height;
    recalculateProjection();

}

void SceneCamera::recalculateProjection()
{
    if (m_ProjectionType == ProjectionType::Prespective)
    {
        m_Projection = glm::perspective(m_PrespectiveFOV, m_AspectRatio, m_PrespectiveNear, m_PrespectiveFar);
    }
    else
    {
        float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
        float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
        float orthoBottom = -m_OrthographicSize * 0.5f;
        float orthoTop = m_OrthographicSize * 0.5f;
        
        m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
    }
}
