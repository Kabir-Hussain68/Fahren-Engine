#include "fhpch.h"
#include "linuxWindow.h"

#include "Engine/events/keyEvent.h"
#include "Engine/events/applicationEvent.h"
#include "Engine/events/mouseEvent.h"

#include "glad/glad.h"

static bool m_GLFWInitialized = false;

static  void GLFWErrorCallback(int error, const char* description)
{
    FH_CORE_ERROR("GLFW ERROR ({0}) : {1}", error, description);
}

Window* Window::createWindow(const windowProps& props)
{
    return new LinuxWindow(props);
}

LinuxWindow::LinuxWindow(const windowProps& props)
{
    Init(props);
}

LinuxWindow::~LinuxWindow()
{
    shutdown();
}

void LinuxWindow::Init(const windowProps& props)
{
    m_Data.title = props.title;
    m_Data.width = props.width;
    m_Data.height = props.height;

    FH_CORE_INFO("Creating Windows {0} {1} {2} ", props.title, props.width, props.height);

    if (!m_GLFWInitialized)
    {
        int success = glfwInit();
        FH_CORE_ASSERT(success, "Could not initialize GLFW");
        glfwSetErrorCallback(GLFWErrorCallback);
        m_GLFWInitialized = true;
    }

    m_Window = glfwCreateWindow((int)props.width, (int)props.height, m_Data.title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(m_Window);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    FH_CORE_ASSERT(status, "Failed to initialize glad");
    glfwSetWindowUserPointer(m_Window, &m_Data);
    setVsync(true);

    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
    {
        windowData& data = *(windowData*)glfwGetWindowUserPointer(window);
        data.width = width;
        data.height = height;

        WindowResizeEvent event(width, height);
        data.eventCallback(event);
    });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
    {
        windowData& data = *(windowData*)glfwGetWindowUserPointer(window);
        WindowCloseEvent event;
        data.eventCallback(event);
    });

    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        windowData& data = *(windowData*)glfwGetWindowUserPointer(window);
 
        switch (action)
        {
            case GLFW_PRESS:
            {
                KeyPressedEvent event(key, 0);
                data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                KeyReleasedEvent event(key);
                data.eventCallback(event);
                break;
            }
            case GLFW_REPEAT:
            {
                KeyPressedEvent event(key, 1);
                data.eventCallback(event);
                break;
            }
        }
    });

    glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keyCode)
    {  
        windowData& data = *(windowData*)glfwGetWindowUserPointer(window);

        KeyTypedEvent event(keyCode);
        data.eventCallback(event);
    });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
    {
        windowData& data = *(windowData*)glfwGetWindowUserPointer(window);

        switch (action)
        {
            case GLFW_PRESS:
            {
                MouseButtonPressedEvent event(button);
                data.eventCallback(event);
                break;
            }
            case GLFW_RELEASE:
            {
                MouseButtonReleasedEvent event(button);
                data.eventCallback(event);
                break;
            }
        }
    });

    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
    {
        windowData& data = *(windowData*)glfwGetWindowUserPointer(window);
        
        MouseScrolledEvent event((float)xOffset, (float)yOffset);
        data.eventCallback(event);
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
    {
        windowData& data = *(windowData*)glfwGetWindowUserPointer(window);
        
        MouseMovedEvent event((float)xPos, (float)yPos);
        data.eventCallback(event);
    });
}

void LinuxWindow::shutdown()
{
    glfwDestroyWindow(m_Window);
}

void LinuxWindow::onUpdate()
{
    glfwPollEvents();
    glfwSwapBuffers(m_Window);
}

void LinuxWindow::setVsync(bool enabled)
{
    if(enabled)
    {
        glfwSwapInterval(1);
    }
    else
    {
        glfwSwapInterval(0);
    }

    m_Data.Vsync = enabled;
}

bool LinuxWindow::isVsync() const
{
    return m_Data.Vsync;
}