#include "fhpch.h"
#include "vertexArray.h"

#include "renderer.h"
#include "Platform/OpenGL/openGLVertexArray.h"

VertexArray *VertexArray::create()
{
    switch (Renderer::getAPI())
    {
        case RendererAPI::None :
            FH_CORE_ASSERT(false, "RenderAPI None currently not supported");
            return nullptr;
        
        case RendererAPI::OpenGL :
            return new OpenGLVertexArray();
    }

    FH_CORE_ASSERT(false, "Unkown Render API");
    return nullptr;
}
