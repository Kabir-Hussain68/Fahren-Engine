#include "fhpch.h"
#include "shader.h"

#include "renderer.h"
#include "Platform/OpenGL/openGLShader.h"

Shader* Shader::create(const std::string& vertexSrx, const std::string& fragmentSrc)
{
    switch (Renderer::getAPI())
    {
        case RendererAPI::API::None :
            FH_CORE_ASSERT(false, "RenderAPI None currently not supported");
            return nullptr;
        
        case RendererAPI::API::OpenGL :
            return new OpenGLShader(vertexSrx, fragmentSrc);
    }

    FH_CORE_ASSERT(false, "Unkown Render API");
    return nullptr;
}
