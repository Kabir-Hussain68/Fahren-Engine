#include "fhpch.h"
#include "imGuiLayer.h"

#include "imgui.h"
#include "Platform/OpenGL/imGuiOpenGLRenderer.h"

#include "Engine/core/application.h"

//Temp
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define FH_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

ImGuiLayer::ImGuiLayer()
    : Layer("ImGui Layer") {}

ImGuiLayer::~ImGuiLayer()
{

}

void ImGuiLayer::onAttach()
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    //Temporary
    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::onDetach()
{

}

void ImGuiLayer::onUpdate()
{
    ImGuiIO& io = ImGui::GetIO();
    Application& app = Application::getApplication();
    io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

    float time = (float)glfwGetTime();
    io.DeltaTime = m_Time > 0.0 ? (time - m_Time) : (1.0 / 60.0);
    m_Time = time;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    static bool showWindow = true;
    ImGui::ShowDemoWindow(&showWindow);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::onEvent(Event& event)
{
    EventDispatcher dispatcher(event);
    dispatcher.dispatch<MouseButtonPressedEvent>(FH_BIND_EVENT_FN(ImGuiLayer::onMouseButtonPressedEvent));
    dispatcher.dispatch<MouseButtonReleasedEvent>(FH_BIND_EVENT_FN(ImGuiLayer::onMouseButtonReleasedEvent));
    dispatcher.dispatch<MouseScrolledEvent>(FH_BIND_EVENT_FN(ImGuiLayer::onMouseScrolledEvent));
    dispatcher.dispatch<MouseMovedEvent>(FH_BIND_EVENT_FN(ImGuiLayer::onMouseMovedEvent));
    dispatcher.dispatch<KeyPressedEvent>(FH_BIND_EVENT_FN(ImGuiLayer::onKeyPressedEvent));
    dispatcher.dispatch<KeyReleasedEvent>(FH_BIND_EVENT_FN(ImGuiLayer::onKeyReleasedEvent));
    dispatcher.dispatch<KeyTypedEvent>(FH_BIND_EVENT_FN(ImGuiLayer::onKeyTypedEvent));
    dispatcher.dispatch<WindowResizeEvent>(FH_BIND_EVENT_FN(ImGuiLayer::onWindowResizeEvent));
}

bool ImGuiLayer::onMouseButtonPressedEvent(MouseButtonPressedEvent& e)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[e.getMouseButton()] = true;

    return false;
}

bool ImGuiLayer::onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[e.getMouseButton()] = false;

    return false;
}

bool ImGuiLayer::onMouseScrolledEvent(MouseScrolledEvent& e)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += e.getOffsetX();
    io.MouseWheel += e.getOffsetY();

    return false;
}

bool ImGuiLayer::onMouseMovedEvent(MouseMovedEvent& e)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(e.getX(), e.getY());

    return false;
}

bool ImGuiLayer::onKeyPressedEvent(KeyPressedEvent& e)
{
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[e.getKeyCode()] = true;

    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

    return false;
}

bool ImGuiLayer::onKeyReleasedEvent(KeyReleasedEvent& e)
{
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[e.getKeyCode()] = false;

    return false;
}

bool ImGuiLayer::onKeyTypedEvent(KeyTypedEvent& e)
{
    ImGuiIO io = ImGui::GetIO();
    int keyCode = e.getKeyCode();

    if (keyCode > 0 && keyCode < 0x10000)
    {
        io.AddInputCharacter((unsigned short)keyCode);
    }

    return false;
}

bool ImGuiLayer::onWindowResizeEvent(WindowResizeEvent& e)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(e.getWidth(), e.getHeight());
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    glViewport(0, 0, e.getWidth(), e.getHeight());

    return false;
}