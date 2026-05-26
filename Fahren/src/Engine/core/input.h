#pragma once

class Input
{
private:
    static Input* m_Instance;

protected:
    virtual bool isKeyPressedImpl(int keycode) = 0;
    
    virtual bool isMouseButtonPressedImpl(int button) = 0;
    virtual std::pair<float, float> getMousePositionImpl() = 0;
    virtual float getMouseXImpl() = 0;
    virtual float getMouseYImpl() = 0;

public:
    inline static bool isKeyPressed(int keycode) { return m_Instance->isKeyPressedImpl(keycode); }
    
    inline static bool isMouseButtonPressed(int button) { return m_Instance->isMouseButtonPressedImpl(button); }
    inline static std::pair<float, float> getMousePosition() { return m_Instance->getMousePositionImpl(); }
    inline static float getMouseX() { return m_Instance->getMouseXImpl(); }
    inline static float getMouseY() { return m_Instance->getMouseYImpl(); }
};