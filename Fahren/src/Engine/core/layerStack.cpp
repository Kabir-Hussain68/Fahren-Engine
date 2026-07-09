#include "fhpch.h"
#include "layerStack.h"

LayerStack::LayerStack()
{
}

LayerStack::~LayerStack()
{
    // Deletes all the layers in the stack
    for (Layer* layer : m_Layers)
    {
        layer->onDetach();
        delete layer;
    }
}

void LayerStack::pushLayer(Layer* layer)
{
    // Pushes the layers in the stack
    m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
    m_LayerInsertIndex++;
}

void LayerStack::pushOverlay(Layer* overlay)
{
    // Pushes overlay at the end of the stack
    m_Layers.emplace_back(overlay);
}

void LayerStack::popLayer(Layer* layer)
{
    // Removes the desired layer from the stack
    // Starts at the begining and loops over all the layers until the index of the last layer
    // If it find the layer it removes it and minus the index
    auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
    if (it != m_Layers.begin() + m_LayerInsertIndex)
    {
        layer->onDetach();
        m_Layers.erase(it);
        m_LayerInsertIndex--;
    }
}

// Removes the desired layer from the stack
// Starts at the ending and loops over all the layers until the beginning
// If it find the layer it removes it 
void LayerStack::popOverlay(Layer* overlay)
{
    auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
    if (it != m_Layers.end())
    {
        overlay->onDetach();
        m_Layers.erase(it);
    }
}