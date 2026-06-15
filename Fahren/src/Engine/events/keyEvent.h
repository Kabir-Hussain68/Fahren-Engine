#pragma once

#include "events.h"
#include "Engine/core/input.h"

class KeyEvent : public Event
{
protected:
    KeyEvent(KeyCode keyCode)
        : m_KeyCode(keyCode) {}
    
    KeyCode m_KeyCode;

public:
    inline KeyCode getKeyCode() const { return m_KeyCode; }

    EVENT_CLASS_CATEGORY(eventCategoryKeyboard | eventCategoryInput);
};

class KeyPressedEvent : public KeyEvent
{
private:
    int m_RepeatCount;

public:
    KeyPressedEvent(KeyCode keyCode, int repeatCount)
        : KeyEvent(keyCode), m_RepeatCount(repeatCount) {}

    inline int getRepeatCount() const { return m_RepeatCount; }

    std::string toString() const override
    {
        std::stringstream ss;
        ss << "KeyPressedEvent : " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
        return ss.str();
    }

    EVENT_CLASS_TYPE(keyPressed)
};

class KeyReleasedEvent : public KeyEvent
{
public:
    KeyReleasedEvent(KeyCode keyCode)
        : KeyEvent(keyCode) {}


    std::string toString() const override
    {
        std::stringstream ss;
        ss << "KeyReleasedEvent : " << m_KeyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(keyReleased)
};

class KeyTypedEvent : public KeyEvent
{
public:
    KeyTypedEvent(KeyCode keyCode)
        : KeyEvent(keyCode) {}

    std::string toString() const override
    {
        std::stringstream ss;
        ss << "KeyTypedEvent : " << m_KeyCode;
        return ss.str();
    }

    EVENT_CLASS_TYPE(keyTyped)
};