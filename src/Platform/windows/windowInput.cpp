#include "fhpch.h"
#include "windowInput.h"

#include "Engine/core/application.h"

#include <GLFW/glfw3.h>

Input* Input::m_Instance = new WindowInput();

bool WindowInput::isKeyPressedImpl(int keycode)
{
    auto window = static_cast<GLFWwindow*>(Application::getApplication().getWindow().getNativeWindow());
    auto state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowInput::isMouseButtonPressedImpl(int button)
{
    auto window = static_cast<GLFWwindow*>(Application::getApplication().getWindow().getNativeWindow());
    auto state = glfwGetKey(window, button);
    return state == GLFW_PRESS;
}

std::pair<float, float> WindowInput::getMousePositionImpl()
{
    auto window = static_cast<GLFWwindow*>(Application::getApplication().getWindow().getNativeWindow());
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    return { (float)xPos, (float)yPos };
}

float WindowInput::getMouseXImpl()
{
    auto [x, y] = getMousePositionImpl();
    return x;
}

float WindowInput::getMouseYImpl()
{
    auto [x, y] = getMousePositionImpl();
    return y;
}
