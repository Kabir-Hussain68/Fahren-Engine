#include "fhpch.h"
#include "openGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

OpenGLContext::OpenGLContext(GLFWwindow *windowHandle)
    : m_WindowHandle(windowHandle)
{
    FH_CORE_ASSERT(m_WindowHandle, "Window Hanle is null");
}

void OpenGLContext::Init()
{
    glfwMakeContextCurrent(m_WindowHandle);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    FH_CORE_ASSERT(status, "Failed to initialize glad");

    FH_CORE_INFO("OpenGL Info :");
    FH_CORE_INFO("  Vendor : {0}" , (const char*)glGetString(GL_VENDOR));
    FH_CORE_INFO("  Renderer : {0}" , (const char*)glGetString(GL_RENDERER));
    FH_CORE_INFO("  Version : {0}" , (const char*)glGetString(GL_VERSION));
}

void OpenGLContext::swapBuffers()
{
    glfwSwapBuffers(m_WindowHandle);
}
