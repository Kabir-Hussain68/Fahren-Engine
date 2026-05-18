#pragma once

#include "window.h"
#include "Engine/events/events.h"
#include "Engine/events/applicationEvent.h"

class Application
{
private:
    std::unique_ptr<Window> m_Window;
    bool m_Running = true;

    bool onWindowClose(WindowCloseEvent& e);
public:
    Application();
    ~Application();

    void run();

    void onEvent(Event& e);
};