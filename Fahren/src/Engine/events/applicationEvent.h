#pragma once

#include "events.h"

class WindowResizeEvent : public Event
{
private:
    unsigned int m_Width, m_Height;

public:
    WindowResizeEvent(unsigned int width, unsigned int height)
        : m_Width(width), m_Height(height) {}
    
    inline unsigned int getWidth() const { return m_Width; }
    inline unsigned int getHeight() const { return m_Height; }

    std::string toString() const override
    {
        std::stringstream ss;
        ss << "WindowResizeEvent : " << m_Width << " , " << m_Height;
        return ss.str();
    }

    EVENT_CLASS_TYPE(windowResize)
    EVENT_CLASS_CATEGORY(eventCategoryApplication)
};

class WindowCloseEvent : public Event
	{
public:
	WindowCloseEvent() = default;

	EVENT_CLASS_TYPE(windowClose)
	EVENT_CLASS_CATEGORY(eventCategoryApplication)
};

class AppTickEvent : public Event
{
public:
	AppTickEvent() = default;

	EVENT_CLASS_TYPE(appTick)
	EVENT_CLASS_CATEGORY(eventCategoryApplication)
};

class AppUpdateEvent : public Event
{
public:
	AppUpdateEvent() = default;

	EVENT_CLASS_TYPE(appUpdate)
	EVENT_CLASS_CATEGORY(eventCategoryApplication)
};

class AppRenderEvent : public Event
{
public:
	AppRenderEvent() = default;

	EVENT_CLASS_TYPE(appRender)
	EVENT_CLASS_CATEGORY(eventCategoryApplication)
};