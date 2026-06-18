#include "fhpch.h"
#include "Engine/core/input.h"

#include "Engine/core/application.h"
#include <GLFW/glfw3.h>

bool Input::isKeyPressed(KeyCode keycode)
{
    auto window = static_cast<GLFWwindow*>(Application::getApplication().getWindow().getNativeWindow());
    auto state = glfwGetKey(window, static_cast<int32_t>(keycode));
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::isMouseButtonPressed(MouseCode button)
{
    auto window = static_cast<GLFWwindow*>(Application::getApplication().getWindow().getNativeWindow());
    auto state = glfwGetKey(window, static_cast<int32_t>(button));
    return state == GLFW_PRESS;
}

std::pair<float, float> Input::getMousePosition()
{
    auto window = static_cast<GLFWwindow*>(Application::getApplication().getWindow().getNativeWindow());
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    return { (float)xPos, (float)yPos };
}

float Input::getMouseX()
{
    auto [x, y] = getMousePosition();
    return x;
}

float Input::getMouseY()
{
    auto [x, y] = getMousePosition();
    return y;
}
