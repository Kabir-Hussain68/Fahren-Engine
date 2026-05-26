#pragma once

class GraphicsContext
{
private:

public:
    virtual void Init() = 0;
    virtual void swapBuffers() = 0;
};