#include "fhpch.h"
#include "openGLShader.h"

#include <fstream>

#include <glad/glad.h>

#include "glm/gtc/type_ptr.hpp"

static GLenum shaderTypeFromString(const std::string& type)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;

    FH_CORE_ASSERT(false, "Unkown Shader type!");
    return 0;
}

OpenGLShader::OpenGLShader(const std::string &path)
{
    std::string source = readFile(path);
    auto shaderSources = preProcess(source);
    compile(shaderSources);

    auto lastSlash = path.find_last_of("/\\");
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
    auto lastDot = path.rfind('.');
    auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;
    m_Name = path.substr(lastSlash, count);
}

OpenGLShader::OpenGLShader(const std::string& name, const std::string &vertexSrc, const std::string &fragmentSrc)
    : m_Name(name)
{
    std::unordered_map<GLenum, std::string> sources;
    sources[GL_VERTEX_SHADER] = vertexSrc;
    sources[GL_FRAGMENT_SHADER] = fragmentSrc;
    compile(sources);
}

OpenGLShader::~OpenGLShader()
{
    glDeleteProgram(m_RendererID);
}

std::string OpenGLShader::readFile(const std::string &path)
{
    std::string result;
    std::ifstream in(path, std::ios::in | std::ios::binary);
    if (in)
    {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
        in.close();
    }
    else
    {
        FH_CORE_ERROR("Could not open file '{0}'", path);
    }

    return result;
}

std::unordered_map<GLenum, std::string> OpenGLShader::preProcess(const std::string &source)
{
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
        FH_CORE_ASSERT(shaderTypeFromString(type), "Invalid Shader type specification!");

        size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        pos = source.find(typeToken, nextLinePos);
        shaderSources[shaderTypeFromString(type)] = 
            source.substr(nextLinePos, 
                pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
    }

    return shaderSources;
}

void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shaderSources)
{
    GLuint program = glCreateProgram();
    FH_CORE_ASSERT(shaderSources.size() <= 2, "Only 2 shaders supported right now");
    std::array<GLenum, 2> glShaderIDs;
    int glShaderIDIndex = 0;

    for (auto& key : shaderSources)
    {
        GLenum shaderType = key.first;
        const std::string& source = key.second;

        GLuint shader = glCreateShader(shaderType);

        const GLchar *sourceCStr = source.c_str();
        glShaderSource(shader, 1, &sourceCStr, 0);

        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if(isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
            
            glDeleteShader(shader);

            FH_CORE_ERROR("{0}", infoLog.data());
            FH_CORE_ASSERT(false, "Shader compilation failed");

            break;
        }

        glAttachShader(program, shader);
        glShaderIDs[glShaderIDIndex++] = shader;
    }

    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
        
        glDeleteProgram(program);

        for (auto& ID : glShaderIDs)
        {
            glDeleteShader(ID);
        }


        FH_CORE_ERROR("{0}", infoLog.data());
        FH_CORE_ASSERT(false, "Shader link failed");

        return;
    }

    for (auto& ID : glShaderIDs)
    {
        glDetachShader(program, ID);
    }

    m_RendererID = program;
}

void OpenGLShader::bind() const
{
    glUseProgram(m_RendererID);
}

void OpenGLShader::unBind() const
{
    glUseProgram(0);
}

void OpenGLShader::uploadUniformInt(const std::string &uniform, int value)
{
    GLint location = glGetUniformLocation(m_RendererID, uniform.c_str());
    glUniform1i(location, value);
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
