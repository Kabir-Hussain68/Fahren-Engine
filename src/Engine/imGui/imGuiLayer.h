#pragma once

#include "Engine/core/layer.h"
#include "Engine/events/mouseEvent.h"
#include "Engine/events/keyEvent.h"
#include "Engine/events/applicationEvent.h"

class ImGuiLayer : public Layer
{
private:
    float m_Time = 0.0f;

    bool onMouseButtonPressedEvent(MouseButtonPressedEvent& e);
    bool onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
    bool onMouseScrolledEvent(MouseScrolledEvent& e);
    bool onMouseMovedEvent(MouseMovedEvent& e);
    bool onKeyPressedEvent(KeyPressedEvent& e);
    bool onKeyReleasedEvent(KeyReleasedEvent& e);
    bool onKeyTypedEvent(KeyTypedEvent& e);
    bool onWindowResizeEvent(WindowResizeEvent& e);
public:
    ImGuiLayer();
    ~ImGuiLayer();

    void onAttach();
    void onDetach();
    void onUpdate();
    void onEvent(Event& event);
};