#include <string>
#include <functional>

#define BIT(x) (1 << x)

//Events in this engine are currently blocking, meaning if an event occurs it is immediately
//dispatched and handled right then and there

enum class EventType
{
    none = 0,
    windowClose, windowResize, windowFocus, windowLostFocus, windowMoved,
    appTick, appUpdate, appRender,
    keyPressed, keyReleased,
    mouseButtonPressed, mouseButtonReleased, mouseMoved, mouseScrolled
};

//Which Category the event belongs to, if i only need mouse pressed give me mouse pressed

enum EventCategory
{
    none = 0,
    eventCategoryApplication    = BIT(0), //00001 = 1
    eventCategoryInput          = BIT(1), //00010 = 2
    eventCategoryKeyboard       = BIT(2), //00100 = 4
    eventCategoryMouse          = BIT(3), //01000 = 8
    eventCategoryMouseButton    = BIT(4)  //10000 = 16
};


//Create a generic macro, rather than overriding the getters we just define a macro
//to use them as such EVENT_CLASS_TYPE(ANYEVENT)
#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::type; }\
                                virtual EventType getEventType() const override { return getStaticType(); }\
                                virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return category; }

class Event
{
    friend class EventDispatcher;

protected:
    bool m_Handled = false;     //If an event has been handled or not

public:
    virtual EventType getEventType() const = 0;
    virtual const char* getName() const = 0;
    virtual int getCategoryFlags() const = 0;
    virtual std::string toString() const { return getName(); }

    inline bool isInCategory(EventCategory categorgy)
    {
        return getCategoryFlags() & categorgy; //If events belongs in the category or not
    }
};

class EventDispatcher
{
    template<typename T>
    using EventFn = std::function<bool(T&)>;

private:
    Event& m_Event;

public:
    EventDispatcher(Event& event)
        : m_Event(event) {}
    
    template<typename T> 
    bool dispatch(EventFn<T> func)
    {
        if(m_Event.getEventType() == T::getStaticType())
        {
            m_Event.m_Handled = func(*(T*)&m_Event);
            return true;
        }
        return false;
    }

};

inline std::ostream& operator<<(std::ostream& os, const Event& e)
{
    return os << e.toString();
}