#pragma once

#include "Engine/events/events.h"

class Layer
{
protected:
    std::string m_DebugName;

public:
    Layer(const std::string& debugName = "Layer");
    virtual ~Layer();

    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onUpdate() {}
    virtual void onImGuiRender() {}
    virtual void onEvent(Event& event) {}

    inline const std::string& getName() const { return m_DebugName; }
};