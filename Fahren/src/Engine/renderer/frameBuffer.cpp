#include "fhpch.h"
#include "frameBuffer.h"

#include "Engine/renderer/renderer.h"

#include "Platform/OpenGL/openGLFrameBuffer.h"

// Checks which API is being used and creates the frame buffer object for that API
Ref<FrameBuffer> FrameBuffer::create(const FrameBufferSpecification &spec)
{
    switch (Renderer::getAPI())
    {
        case RendererAPI::API::None :
            FH_CORE_ASSERT(false, "RenderAPI None currently not supported");
            return nullptr;
        
        case RendererAPI::API::OpenGL :
            return createRef<OpenGLFrameBuffer>(spec);
    }

    FH_CORE_ASSERT(false, "Unkown Render API");
    return nullptr;}
