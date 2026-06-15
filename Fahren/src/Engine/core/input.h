#pragma once

#include "base.h"
#include "keyCodes.h"
#include "mouseCodes.h"

class Input
{
private:
    static Scope<Input> m_Instance;

protected:
    virtual bool isKeyPressedImpl(KeyCode key) = 0;
    
    virtual bool isMouseButtonPressedImpl(MouseCode button) = 0;
    virtual std::pair<float, float> getMousePositionImpl() = 0;
    virtual float getMouseXImpl() = 0;
    virtual float getMouseYImpl() = 0;

public:
    inline static bool isKeyPressed(KeyCode key) { return m_Instance->isKeyPressedImpl(key); }
    
    inline static bool isMouseButtonPressed(MouseCode button) { return m_Instance->isMouseButtonPressedImpl(button); }
    inline static std::pair<float, float> getMousePosition() { return m_Instance->getMousePositionImpl(); }
    inline static float getMouseX() { return m_Instance->getMouseXImpl(); }
    inline static float getMouseY() { return m_Instance->getMouseYImpl(); }
};