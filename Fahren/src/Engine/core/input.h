#pragma once

#include <glm/glm.hpp>

#include "keyCodes.h"
#include "mouseCodes.h"

using namespace Mouse;

// Static class with only declaration, definition in implementation class
class Input
{
private:
    static Scope<Input> m_Instance;

public: 
    static bool isKeyPressed(KeyCode key);
    
    static bool isMouseButtonPressed(MouseCode button);
    static glm::vec2 getMousePosition();
    static float getMouseX();
    static float getMouseY();
};