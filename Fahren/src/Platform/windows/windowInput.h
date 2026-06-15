#pragma once

#include "Engine/core/input.h"

class WindowInput : public Input
{
protected:
    virtual bool isKeyPressedImpl(KeyCode keycode) override;

    virtual bool isMouseButtonPressedImpl(MouseCode button) override;
    virtual std::pair<float, float> getMousePositionImpl() override;
    virtual float getMouseXImpl() override;
    virtual float getMouseYImpl() override;
};