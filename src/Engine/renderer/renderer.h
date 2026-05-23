#pragma once

enum class RendererAPI
{
    None = 0, OpenGL = 1
};

class Renderer
{
private:
    static RendererAPI m_RendererApi;

public:
    inline static RendererAPI getAPI() { return m_RendererApi; }
};
