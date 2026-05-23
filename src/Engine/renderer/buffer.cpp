#include "fhpch.h"
#include "buffer.h"

#include "renderer.h"

#include "Platform/OpenGL/openGLBuffer.h"

VertexBuffer* VertexBuffer::create(float *vertices, uint32_t size)
{
    switch (Renderer::getAPI())
    {
        case RendererAPI::API::None :
            FH_CORE_ASSERT(false, "RenderAPI None currently not supported");
            return nullptr;
        
        case RendererAPI::API::OpenGL :
            return new OpenGLVertexBuffer(vertices, size);
    }

    FH_CORE_ASSERT(false, "Unkown Render API");
    return nullptr;
}

IndexBuffer* IndexBuffer::create(uint32_t* indices, uint32_t size)
{
    switch (Renderer::getAPI())
    {
        case RendererAPI::API::None :
            FH_CORE_ASSERT(false, "RenderAPI None currently not supported");
            return nullptr;
        
        case RendererAPI::API::OpenGL :
            return new OpenGLIndexBuffer(indices, size);
    }

    FH_CORE_ASSERT(false, "Unkown Render API");
    return nullptr;
}
