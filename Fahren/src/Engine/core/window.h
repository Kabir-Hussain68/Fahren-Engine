#include "Engine/events/events.h"

struct windowProps
{
    std::string title;
    uint32_t width;
    uint32_t height;

    windowProps(const std::string& title = "Fahren Engine", 
                uint32_t width = 1920,
                uint32_t height = 1080 )
        : title(title), width(width), height(height)    {}
};

// A virtual class with only function definitions
class Window
{
public:
    using EventCallBackFn = std::function<void(Event&)>;

    virtual ~Window() {}

    virtual void onUpdate() = 0;

    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;

    virtual void setEventCallback(const EventCallBackFn& callback) = 0;
    virtual void setVsync(bool enabled) = 0;
    virtual bool isVsync() const = 0;

    virtual void* getNativeWindow() const = 0;

    static Scope<Window> createWindow(const windowProps& props = windowProps());
};