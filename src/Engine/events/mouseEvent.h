#include "events.h"

#include <sstream>

class MouseMovedEvent : public Event
{
private:
    float m_MouseX, m_MouseY;

public:
    MouseMovedEvent(float mouseX, int mouseY)
        : m_MouseX(mouseX), m_MouseY(mouseY) {}
    
    inline float getX() const { return m_MouseX; }
    inline float getY() const { return m_MouseY; }

    std::string toString() const override
    {
        std::stringstream ss;
        ss << "MouseMovedEvent : " << m_MouseX << " , " << m_MouseY;
        return ss.str();
    }

    EVENT_CLASS_TYPE(mouseMoved)
    EVENT_CLASS_CATEGORY(eventCategoryMouse | eventCategoryInput)
};

class MouseScrolledEvent : public Event
{
private:
    float m_OffsetX, m_OffsetY;

public:
    MouseScrolledEvent(float offsetX, float offsetY)
        : m_OffsetX(offsetX), m_OffsetY(offsetY) {}

    inline float getOffsetX() const { return m_OffsetX; }
    inline float getOffsetY() const { return m_OffsetY; }

    std::string toString() const override
        {
            std::stringstream ss;
            ss << "MouseScrolledEvent : " << m_OffsetX << " , " << m_OffsetY;
            return ss.str();
        }

    EVENT_CLASS_TYPE(mouseScrolled)
    EVENT_CLASS_CATEGORY(eventCategoryMouse | eventCategoryInput)
};

class MouseButtonEvent : public Event
{
protected:
    int m_Button;

    MouseButtonEvent(int button)
        : m_Button(button) {}

public:
    inline int getMouseButton() const { return m_Button; }

    EVENT_CLASS_CATEGORY(eventCategoryMouse | eventCategoryInput)
};

class MouseButtonPressedEvent : public MouseButtonEvent
{
public:
	MouseButtonPressedEvent(int button)
		: MouseButtonEvent(button) {}

	std::string toString() const override
	{
		std::stringstream ss;
		ss << "MouseButtonPressedEvent: " << m_Button;
		return ss.str();
	}

	EVENT_CLASS_TYPE(mouseButtonPressed)
};

class MouseButtonReleasedEvent : public MouseButtonEvent
{
public:
	MouseButtonReleasedEvent(int button)
		: MouseButtonEvent(button) {}

	std::string toString() const override
	{
		std::stringstream ss;
		ss << "MouseButtonReleasedEvent: " << m_Button;
		return ss.str();
	}

	EVENT_CLASS_TYPE(mouseButtonReleased)
};


