#include "fhpch.h"
#include "texture.h"

#include "renderer.h"
#include "Platform/OpenGL/openGLTexture.h"

// Creates the particular texture based on the API
Ref<Texture2D> Texture2D::create(uint32_t width, uint32_t height)
{
    switch (Renderer::getAPI())
    {
        case RendererAPI::API::None :
            FH_CORE_ASSERT(false, "RenderAPI None currently not supported");
            return nullptr;
        
        case RendererAPI::API::OpenGL :
            return createRef<OpenGLTexture2D>(width, height);
    }

    FH_CORE_ASSERT(false, "Unkown Render API");
    return nullptr;}

Ref<Texture2D> Texture2D::create(const std::string &path)
{
    switch (Renderer::getAPI())
    {
        case RendererAPI::API::None :
            FH_CORE_ASSERT(false, "RenderAPI None currently not supported");
            return nullptr;
        
        case RendererAPI::API::OpenGL :
            return createRef<OpenGLTexture2D>(path);
    }

    FH_CORE_ASSERT(false, "Unkown Render API");
    return nullptr;
}
