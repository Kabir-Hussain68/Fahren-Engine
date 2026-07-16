#pragma once

#include <glm/glm.hpp>
#include "vertexArray.h"

// A class which selects which API is currently being used
// A virtual class with definitions in the declaration
class RendererAPI
{
private:

public:
    enum class API
    {
        None = 0, OpenGL = 1
    };

    virtual ~RendererAPI() = default;

    virtual void Init() = 0;
    virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    virtual void setClearColor(const glm::vec4& color) = 0;
    virtual void clear() = 0;

    virtual void drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
    virtual void drawLines(const Ref<VertexArray> &vertexArray, uint32_t vertexCount) = 0;

    virtual void setLineThickness(float thickness) = 0;

    inline static API getAPI() { return s_API; }
    static Scope<RendererAPI> create();

private:
    static API s_API;
};