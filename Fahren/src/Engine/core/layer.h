#pragma once

#include "Engine/events/events.h"
#include "Engine/core/timestep.h"

class Layer
{
protected:
    std::string m_DebugName;

public:
    Layer(const std::string& debugName = "Layer");
    virtual ~Layer();

    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onUpdate(Timestep ts) {}
    virtual void onImGuiRender() {}
    virtual void onEvent(Event& event) {}

    inline const std::string& getName() const { return m_DebugName; }
};