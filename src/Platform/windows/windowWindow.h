#pragma once

#include "Engine/core/window.h"

#include "GLFW/glfw3.h"

class LinuxWindow : public Window
{
private:
    GLFWwindow* m_Window;

    struct windowData
    {
        std::string title;
        unsigned int width;
        unsigned int height;
        bool Vsync;

        EventCallBackFn eventCallback;
    };

    windowData m_Data;

    virtual void Init(const windowProps& props);
    virtual void shutdown();

public:
    LinuxWindow(const windowProps& props);
    virtual ~LinuxWindow();

    void onUpdate() override;

    inline unsigned int getWidth() const override { return m_Data.width; }
    inline unsigned int getHeight() const override { return m_Data.height; }

    inline void setEventCallback(const EventCallBackFn& callback) override { m_Data.eventCallback = callback; }
    void setVsync(bool enabled) override;
    bool isVsync() const override;

    inline virtual void* getNativeWindow() const { return m_Window; };
};