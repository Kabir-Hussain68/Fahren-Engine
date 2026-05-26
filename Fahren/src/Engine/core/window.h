#include "Engine/events/events.h"

struct windowProps
{
    std::string title;
    unsigned int width;
    unsigned int height;

    windowProps(const std::string& title = "Fahren Engine", 
                unsigned int width = 1280,
                unsigned int height = 720 )
        : title(title), width(width), height(height)    {}
};

class Window
{
public:
    using EventCallBackFn = std::function<void(Event&)>;

    virtual ~Window() {}

    virtual void onUpdate() = 0;

    virtual unsigned int getWidth() const = 0;
    virtual unsigned int getHeight() const = 0;

    virtual void setEventCallback(const EventCallBackFn& callback) = 0;
    virtual void setVsync(bool enabled) = 0;
    virtual bool isVsync() const = 0;

    virtual void* getNativeWindow() const = 0;

    static Window* createWindow(const windowProps& props = windowProps());
};