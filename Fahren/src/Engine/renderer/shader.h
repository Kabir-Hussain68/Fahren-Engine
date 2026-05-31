#pragma once

#include <string>

#include "glm/glm.hpp"

class Shader
{
private:
    uint32_t m_RendererID;

public:
    Shader(const std::string& vertexSrx, const std::string& fragmentSrc);
    ~Shader();

    void bind() const;
    void unBind() const;

    void uploadUniform(const std::string& uniform, const glm::mat4& viewProjectionMatrix);
};