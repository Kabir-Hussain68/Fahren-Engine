#include "fhpch.h"
#include "orthographicCameraController.h"

#include "Engine/core/input.h"
#include "Engine/core/keyCodes.h"

OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
    : m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel), m_Rotation(rotation)
{

}

void OrthographicCameraController::onUpdate(Timestep ts)
{
    if(Input::isKeyPressed(FH_KEY_LEFT))
    {
        m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
    }
    if(Input::isKeyPressed(FH_KEY_RIGHT))
    {
        m_CameraPosition.x += m_CameraTranslationSpeed * ts;
    }
    if(Input::isKeyPressed(FH_KEY_UP))
    {
        m_CameraPosition.y += m_CameraTranslationSpeed * ts;
    }
    if(Input::isKeyPressed(FH_KEY_DOWN))
    {
        m_CameraPosition.y -= m_CameraTranslationSpeed * ts;
    }
        
    if (m_Rotation)
    {
        if(Input::isKeyPressed(FH_KEY_A))
        {
            m_CameraRotation += m_CameraRotationSpeed * ts;
        }
    
        if(Input::isKeyPressed(FH_KEY_D))
        {
            m_CameraRotation -= m_CameraRotationSpeed * ts;
        }
        m_Camera.setRotation(m_CameraRotation);
    }

    m_Camera.setPosition(m_CameraPosition);
    
    m_CameraTranslationSpeed = m_ZoomLevel;
}

void OrthographicCameraController::onEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.dispatch<MouseScrolledEvent>(FH_BIND_EVENT_FN(onMouseScrolled));
    dispatcher.dispatch<WindowResizeEvent>(FH_BIND_EVENT_FN(onWindowResized));
}

bool OrthographicCameraController::onMouseScrolled(MouseScrolledEvent& event)
{
    m_ZoomLevel -= event.getOffsetY() * 0.25f;
    m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
    m_Camera.setProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    return false;
}

bool OrthographicCameraController::onWindowResized(WindowResizeEvent& event)
{
    m_AspectRatio = (float)event.getWidth() / (float)event.getHeight();
    m_Camera.setProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    return false;
}
