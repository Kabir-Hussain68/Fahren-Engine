#include "fhpch.h"
#include "openGLShader.h"

#include <fstream>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "Engine/core/timer.h"

namespace Utils
{
    static GLenum shaderTypeFromString(const std::string& type)
    {
        if (type == "vertex")
            return GL_VERTEX_SHADER;
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;
    
        FH_CORE_ASSERT(false, "Unkown Shader type!");
        return 0;
    }

    static shaderc_shader_kind glShaderStageToShaderC(GLenum stage)
    {
        switch (stage)
        {
            case GL_VERTEX_SHADER:
                return shaderc_glsl_vertex_shader;
            
            case GL_FRAGMENT_SHADER:
                return shaderc_glsl_fragment_shader;
        }

        FH_CORE_ASSERT(false);
        return (shaderc_shader_kind)0;
    }

    static const char* glShaderStageToString(GLenum stage)
    {
        switch (stage)
        {
            case GL_VERTEX_SHADER:
                return "GL_VERTEX_SHADER";
            
            case GL_FRAGMENT_SHADER:
                return "GL_FRAGMENT_SHADER";
        }

        FH_CORE_ASSERT(false);
        return nullptr;
    }

    static const char* getCacheDirectory()
    {
        return "assets/cache/shader/opengl";
    }

    static void createCacheDirectoryIfNeeded()
    {
        std::string cacheDirectory = getCacheDirectory();
        if (!std::filesystem::exists(cacheDirectory))
        {
            std::filesystem::create_directories(cacheDirectory);
        }
    }

    static const char* glShaderStageCachedOpenGLFileExtension(uint32_t stage)
    {
        switch (stage)
        {
            case GL_VERTEX_SHADER:    
                return ".cached_opengl.vert";
            case GL_FRAGMENT_SHADER:  
                return ".cached_opengl.frag";
        }

		FH_CORE_ASSERT(false);
		return "";
    }

    static const char* glShaderStageCachedVulkanFileExtension(uint32_t stage)
    {
        switch (stage)
        {
        case GL_VERTEX_SHADER:    
            return ".cached_vulkan.vert";
        case GL_FRAGMENT_SHADER:  
            return ".cached_vulkan.frag";
        }

        FH_CORE_ASSERT(false);
        return "";
    }

}


OpenGLShader::OpenGLShader(const std::string &path)
    : m_FilePath(path)
{
    FH_PROFILE_FUNCTION();

    Utils::createCacheDirectoryIfNeeded();

    std::string source = readFile(path);
    auto shaderSources = preProcess(source);

    {
        Timer timer;
        compileOrGetVulkanBinaries(shaderSources);
        compileOrGetOpenGLBinaries();
        createProgram();
        FH_CORE_WARN("Shader Construction took {0} ms", timer.elapsedMillis());
    }

    auto lastSlash = path.find_last_of("/\\");
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
    auto lastDot = path.rfind('.');
    auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;
    m_Name = path.substr(lastSlash, count);
}

OpenGLShader::OpenGLShader(const std::string& name, const std::string &vertexSrc, const std::string &fragmentSrc)
    : m_Name(name)
{
    FH_PROFILE_FUNCTION();

    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertexSrc;
    sources[GL_FRAGMENT_SHADER] = fragmentSrc;

    compileOrGetVulkanBinaries(sources);
    compileOrGetOpenGLBinaries();
    createProgram();
}

OpenGLShader::~OpenGLShader()
{
    FH_PROFILE_FUNCTION();

    glDeleteProgram(m_RendererID);
}

std::string OpenGLShader::readFile(const std::string &path)
{
    FH_PROFILE_FUNCTION();

    std::string result;
    std::ifstream in(path, std::ios::in | std::ios::binary); //Ifstream closes itself due to RAII
    if (in)
    {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
    }
    else
    {
        FH_CORE_ERROR("Could not open file '{0}'", path);
    }

    return result;
}

std::unordered_map<GLenum, std::string> OpenGLShader::preProcess(const std::string &source)
{
    FH_PROFILE_FUNCTION();

    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken = "#type";
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);
    while (pos != std::string::npos)
    {
        size_t eol = source.find_first_of("\r\n", pos);
        FH_CORE_ASSERT(eol != std::string::npos, "Syntax Error");
        size_t begin = pos + typeTokenLength + 1;
        std::string type = source.substr(begin, eol - begin);
        FH_CORE_ASSERT(Utils::shaderTypeFromString(type), "Invalid Shader type specification!");

        size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        pos = source.find(typeToken, nextLinePos);
        FH_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
        
        shaderSources[Utils::shaderTypeFromString(type)] = 
            (pos == std::string::npos) ? source.substr(nextLinePos) : 
            source.substr(nextLinePos, pos - nextLinePos);
    }

    return shaderSources;
}

void OpenGLShader::compileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources)
{

    GLuint program = glCreateProgram();

    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
    const bool optimize = true;
    if (optimize)
    {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }

    std::filesystem::path cacheDirectory = Utils::getCacheDirectory();

    auto& shaderData = m_VulkanSPIRV;
    shaderData.clear();
    for (auto&& [stage, source] : shaderSources)
    {
        std::filesystem::path shaderFilePath = m_FilePath;
        std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::glShaderStageCachedVulkanFileExtension(stage));

        std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
        if (in.is_open())
        {
            in.seekg(0, std::ios::end);
            auto size = in.tellg();
            in.seekg(0, std::ios::beg);

            auto& data = shaderData[stage];
            data.resize(size / sizeof(uint32_t));
            in.read((char*)data.data(), size);
        }
        else
        {
            shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::glShaderStageToShaderC(stage), m_FilePath.c_str(), options);
            if (module.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                FH_CORE_ERROR(module.GetErrorMessage());
                FH_CORE_ASSERT(false);
            }

            shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

            std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
            if (out.is_open())
            {
                auto& data = shaderData[stage];
                out.write((char*)data.data(), data.size() * sizeof(uint32_t));
                out.flush();
                out.close();
            }
        }
    }

    for (auto&& [stage, data]: shaderData)
        reflect(stage, data);
}

void OpenGLShader::compileOrGetOpenGLBinaries()
{
    auto& shaderData = m_OpenGLSPIRV;

    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
    const bool optimize = false;
    if (optimize)
    {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }

    std::filesystem::path cacheDirectory = Utils::getCacheDirectory();

    shaderData.clear();
    m_OpenGLSourceCode.clear();
    for (auto&& [stage, spirv] : m_VulkanSPIRV)
    {
        std::filesystem::path shaderFilePath = m_FilePath;
        std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::glShaderStageCachedOpenGLFileExtension(stage));

        std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
        if (in.is_open())
        {
            in.seekg(0, std::ios::end);
            auto size = in.tellg();
            in.seekg(0, std::ios::beg);

            auto& data = shaderData[stage];
            data.resize(size / sizeof(uint32_t));
            in.read((char*)data.data(), size);
        }
        else
        {
            spirv_cross::CompilerGLSL glslCompiler(spirv);
            m_OpenGLSourceCode[stage] = glslCompiler.compile();
            auto& source = m_OpenGLSourceCode[stage];

            shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::glShaderStageToShaderC(stage), m_FilePath.c_str());
            if (module.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                FH_CORE_ERROR(module.GetErrorMessage());
                FH_CORE_ASSERT(false);
            }

            shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

            std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
            if (out.is_open())
            {
                auto& data = shaderData[stage];
                out.write((char*)data.data(), data.size() * sizeof(uint32_t));
                out.flush();
                out.close();
            }
        }
    }
}

void OpenGLShader::createProgram()
{
    GLuint program = glCreateProgram();

    std::vector<GLuint> shaderIDs;
    for (auto&& [stage, spirv] : m_OpenGLSPIRV)
    {
        GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
		glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
		glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
		glAttachShader(program, shaderID);
    }

    glLinkProgram(program);

    GLint isLinked;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
        FH_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_FilePath, infoLog.data());

        glDeleteProgram(program);

        for (auto id : shaderIDs)
            glDeleteShader(id);
    }

    for (auto id : shaderIDs)
    {
        glDetachShader(program, id);
        glDeleteShader(id);
    }

    m_RendererID = program;
}

void OpenGLShader::reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
{
    spirv_cross::Compiler compiler(shaderData);
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    FH_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::glShaderStageToString(stage), m_FilePath);
    FH_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
    FH_CORE_TRACE("    {0} resources", resources.sampled_images.size());

    FH_CORE_TRACE("Uniform buffers:");
    for (const auto& resource : resources.uniform_buffers)
    {
        const auto& bufferType = compiler.get_type(resource.base_type_id);
        uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
        uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
        int memberCount = bufferType.member_types.size();

        FH_CORE_TRACE("  {0}", resource.name);
        FH_CORE_TRACE("    Size = {0}", bufferSize);
        FH_CORE_TRACE("    Binding = {0}", binding);
        FH_CORE_TRACE("    Members = {0}", memberCount);
    }
}

void OpenGLShader::bind() const
{
    FH_PROFILE_FUNCTION();

    glUseProgram(m_RendererID);
}

void OpenGLShader::unBind() const
{
    FH_PROFILE_FUNCTION();

    glUseProgram(0);
}

void OpenGLShader::setMat4(const std::string& name, const glm::mat4 &value)
{
    FH_PROFILE_FUNCTION();

    uploadUniformMat4(name, value);
}

void OpenGLShader::setInt(const std::string &name, int value)
{
    FH_PROFILE_FUNCTION();

    uploadUniformInt(name, value);
}

void OpenGLShader::setIntArray(const std::string &name, int* values, uint32_t count)
{
    FH_PROFILE_FUNCTION();

    uploadUniformIntArray(name, values, count);
}

void OpenGLShader::setFloat(const std::string &name, float value)
{
    FH_PROFILE_FUNCTION();

    uploadUniformFloat(name, value);
}

void OpenGLShader::setFloat2(const std::string& name, const glm::vec2& value)
{
	FH_PROFILE_FUNCTION();

	uploadUniformFloat2(name, value);
}

void OpenGLShader::setFloat3(const std::string& name, const glm::vec3 &value)
{
    FH_PROFILE_FUNCTION();

    uploadUniformFloat3(name, value);
}

void OpenGLShader::setFloat4(const std::string& name, const glm::vec4 &value)
{
    FH_PROFILE_FUNCTION();
    
    uploadUniformFloat4(name, value);
}

void OpenGLShader::uploadUniformInt(const std::string &uniform, int value)
{
    GLint location = glGetUniformLocation(m_RendererID, uniform.c_str());
    glUniform1i(location, value);
}

void OpenGLShader::uploadUniformIntArray(const std::string &uniform, int* values, uint32_t count)
{
    GLint location = glGetUniformLocation(m_RendererID, uniform.c_str());
    glUniform1iv(location, count, values);  
}

void OpenGLShader::uploadUniformFloat(const std::string &uniform, float value)
{
    GLint location = glGetUniformLocation(m_RendererID, uniform.c_str());
    glUniform1f(location, value);
}

void OpenGLShader::uploadUniformFloat2(const std::string &uniform, const glm::vec2 &value)
{
    GLint location = glGetUniformLocation(m_RendererID, uniform.c_str());
    glUniform2f(location, value.x, value.y);
}

void OpenGLShader::uploadUniformFloat3(const std::string &uniform, const glm::vec3 &value)
{
    GLint location = glGetUniformLocation(m_RendererID, uniform.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}

void OpenGLShader::uploadUniformFloat4(const std::string &uniform, const glm::vec4 &value)
{
    GLint location = glGetUniformLocation(m_RendererID, uniform.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void OpenGLShader::uploadUniformMat3(const std::string &uniform, const glm::mat3 &matrix)
{
    GLint location = glGetUniformLocation(m_RendererID, uniform.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::uploadUniformMat4(const std::string &uniform, const glm::mat4 &matrix)
{
    GLint location = glGetUniformLocation(m_RendererID, uniform.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
