#pragma once

#include "Engine/core/layer.h"
#include "Engine/events/mouseEvent.h"
#include "Engine/events/keyEvent.h"
#include "Engine/events/applicationEvent.h"

class ImGuiLayer : public Layer
{
private:
    float m_Time = 0.0f;

public:
    ImGuiLayer();
    ~ImGuiLayer();

    virtual void onAttach() override;
    virtual void onDetach() override;
    virtual void onEvent(Event& e) override;

    void begin();
    void end();
};