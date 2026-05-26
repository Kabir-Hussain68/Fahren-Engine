#pragma once

#include <string>

class Shader
{
private:
    uint32_t m_RendererID;

public:
    Shader(const std::string& vertexSrx, const std::string& fragmentSrc);
    ~Shader();

    void bind() const;
    void unBind() const;

};