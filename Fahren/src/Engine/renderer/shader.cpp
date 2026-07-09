#include "fhpch.h"
#include "shader.h"

#include "renderer.h"
#include "Platform/OpenGL/openGLShader.h"

// Loads the shader just by the path 
Ref<Shader> Shader::create(const std::string &path)
{
    switch (Renderer::getAPI())
    {
        case RendererAPI::API::None :
            FH_CORE_ASSERT(false, "RenderAPI None currently not supported");
            return nullptr;
        
        case RendererAPI::API::OpenGL :
            return createRef<OpenGLShader>(path);
    }

    FH_CORE_ASSERT(false, "Unkown Render API");
    return nullptr; 
}

// Creates a shader by taking its name, vertex and fragment 
Ref<Shader> Shader::create(const std::string& name, const std::string& vertexSrx, const std::string& fragmentSrc)
{
    switch (Renderer::getAPI())
    {
        case RendererAPI::API::None :
            FH_CORE_ASSERT(false, "RenderAPI None currently not supported");
            return nullptr;
        
        case RendererAPI::API::OpenGL :
            return createRef<OpenGLShader>(name, vertexSrx, fragmentSrc);
    }

    FH_CORE_ASSERT(false, "Unkown Render API");
    return nullptr;
}

// Adds the shader to shader map with a name
void ShaderLibrary::add(const std::string &name, const Ref<Shader> &shader)
{
    FH_CORE_ASSERT(exists(name), "Shader already exists!");
    m_Shaders[name] = shader;
}

void ShaderLibrary::add(const Ref<Shader> &shader)
{
    auto& name = shader->getName();
    add(name, shader);
}

// Loads the shader from shader map with a name
Ref<Shader> ShaderLibrary::load(const std::string &path)
{
    auto shader = Shader::create(path);
    add(shader);
    return shader;
}

Ref<Shader> ShaderLibrary::load(const std::string &name, const std::string &path)
{
    auto shader = Shader::create(path);
    add(name, shader);
    return shader;
}

// Returns the required shader
Ref<Shader> ShaderLibrary::get(const std::string &name)
{
    FH_CORE_ASSERT(!exists(name), "Shader not found!");
    return m_Shaders[name];
}

bool ShaderLibrary::exists(const std::string &name) const
{
    return m_Shaders.find(name) == m_Shaders.end();
}
