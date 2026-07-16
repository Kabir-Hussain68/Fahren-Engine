#pragma once

#include "rendererAPI.h"

class RenderCommand
{
private:
    static Scope<RendererAPI> s_RendererAPI;

public:
    // Asks which renderAPI is currently being used.
    // Render Command deals with :
    // Initializing the API
    inline static void Init()
    {        
        s_RendererAPI->Init();
    }

    // Setting the Viewport size
    inline static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        s_RendererAPI->setViewport(x, y, width, height);
    }

    // Setting the viewport color 
    inline static void setClearColor(const glm::vec4& color)
    {
        s_RendererAPI->setClearColor(color);
    }

    // Clearing the buffers
    inline static void clear()
    {
        s_RendererAPI->clear();
    }

    // Drawing the objects on the screen by the VAO
    inline static void drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0)
    {
        s_RendererAPI->drawIndexed(vertexArray, indexCount);
    }

    // Drawing the lines on the screen by the VAO
    inline static void drawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
    {
        s_RendererAPI->drawLines(vertexArray, vertexCount);
    }

    // Drawing the lines on the screen by the VAO
    inline static void setLineThickness(float thickness)
    {
        s_RendererAPI->setLineThickness(thickness);
    }

};