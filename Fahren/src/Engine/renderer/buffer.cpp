#include "fhpch.h"
#include "buffer.h"

#include "renderer.h"

#include "Platform/OpenGL/openGLBuffer.h"

// Checks which API is being used and creates the buffer object for that API
Ref<VertexBuffer> VertexBuffer::create(uint32_t size)
{
    switch (Renderer::getAPI())
    {
        case RendererAPI::API::None :
            FH_CORE_ASSERT(false, "RenderAPI None currently not supported");
            return nullptr;
        
        case RendererAPI::API::OpenGL :
            return createRef<OpenGLVertexBuffer>(size);
    }

    FH_CORE_ASSERT(false, "Unkown Render API");
    return nullptr;
}

// Checks which API is being used and creates the buffer object for that API
Ref<VertexBuffer> VertexBuffer::create(float *vertices, uint32_t size)
{
    switch (Renderer::getAPI())
    {
        case RendererAPI::API::None :
            FH_CORE_ASSERT(false, "RenderAPI None currently not supported");
            return nullptr;
        
        case RendererAPI::API::OpenGL :
            return createRef<OpenGLVertexBuffer>(vertices, size);
    }

    FH_CORE_ASSERT(false, "Unkown Render API");
    return nullptr;
}

// Checks which API is being used and creates the buffer object for that API
Ref<IndexBuffer> IndexBuffer::create(uint32_t* indices, uint32_t size)
{
    switch (Renderer::getAPI())
    {
        case RendererAPI::API::None :
            FH_CORE_ASSERT(false, "RenderAPI None currently not supported");
            return nullptr;
        
        case RendererAPI::API::OpenGL :
            return createRef<OpenGLIndexBuffer>(indices, size);
    }

    FH_CORE_ASSERT(false, "Unkown Render API");
    return nullptr;
}
