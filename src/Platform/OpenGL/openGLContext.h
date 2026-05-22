#pragma once

#include "Engine/renderer/graphicsContext.h"

struct GLFWwindow;

class OpenGLContext : public GraphicsContext
{
private:
    GLFWwindow* m_WindowHandle;

public:
    OpenGLContext(GLFWwindow* windowHandle);

    virtual void Init() override;
    virtual void swapBuffers() override;
};