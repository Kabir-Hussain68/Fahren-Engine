#pragma once

#include "base.h"
#include "keyCodes.h"
#include "mouseCodes.h"

class Input
{
private:
    static Scope<Input> m_Instance;

public: 
    static bool isKeyPressed(KeyCode key);
    
    static bool isMouseButtonPressed(MouseCode button);
    static std::pair<float, float> getMousePosition();
    static float getMouseX();
    static float getMouseY();
};