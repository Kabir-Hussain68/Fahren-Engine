#pragma once

#include "Engine/renderer/shader.h"

#include "glm/glm.hpp"

//Temporary
typedef unsigned int GLenum;

class OpenGLShader : public Shader
{
private:
    uint32_t m_RendererID;
    std::string m_Name;

    std::string readFile(const std::string& path);
    std::unordered_map<GLenum, std::string> preProcess(const std::string& source);
    void compile(const std::unordered_map<GLenum, std::string>& shaderSources);

public:
    OpenGLShader(const std::string& path);
    OpenGLShader(const std::string& name, const std::string& vertexSrx, const std::string& fragmentSrc);
    virtual ~OpenGLShader();

    virtual void bind() const override;
    virtual void unBind() const override;

    virtual void setMat4(const std::string& name, const glm::mat4& value) override;
    virtual void setInt(const std::string& name, int value) override;
    virtual void setFloat3(const std::string& name, const glm::vec3& value) override;
    virtual void setFloat4(const std::string& name, const glm::vec4& value) override;

    virtual const std::string& getName() const override { return m_Name; }

    void uploadUniformInt(const std::string& uniform, int value);

    void uploadUniformFloat(const std::string& uniform, float value);
    void uploadUniformFloat2(const std::string& uniform, const glm::vec2& value);
    void uploadUniformFloat3(const std::string& uniform, const glm::vec3& value);
    void uploadUniformFloat4(const std::string& uniform, const glm::vec4& value);

    void uploadUniformMat3(const std::string& uniform, const glm::mat3& matrix);
    void uploadUniformMat4(const std::string& uniform, const glm::mat4& matrix);
};