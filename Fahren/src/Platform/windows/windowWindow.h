#pragma once

#include "Engine/core/window.h"
#include "Engine/renderer/graphicsContext.h"

#include "GLFW/glfw3.h"


class windowWindow : public Window
{
private:
    GLFWwindow* m_Window;
    GraphicsContext* m_Context;

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
    windowWindow(const windowProps& props);
    virtual ~windowWindow();

    void onUpdate() override;

    inline unsigned int getWidth() const override { return m_Data.width; }
    inline unsigned int getHeight() const override { return m_Data.height; }

    inline void setEventCallback(const EventCallBackFn& callback) override { m_Data.eventCallback = callback; }
    void setVsync(bool enabled) override;
    bool isVsync() const override;

    inline virtual void* getNativeWindow() const { return m_Window; };
};