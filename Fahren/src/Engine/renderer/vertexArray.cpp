#include "fhpch.h"
#include "vertexArray.h"

#include "renderer.h"
#include "Platform/OpenGL/openGLVertexArray.h"

// Checks which API is currently being used
Ref<VertexArray> VertexArray::create()
{    
    switch (Renderer::getAPI())
    {
        case RendererAPI::API::None :
            FH_CORE_ASSERT(false, "RenderAPI None currently not supported");
            return nullptr;
        
        case RendererAPI::API::OpenGL :
            return createRef<OpenGLVertexArray>();
    }

    FH_CORE_ASSERT(false, "Unkown Render API");
    return nullptr;
}
