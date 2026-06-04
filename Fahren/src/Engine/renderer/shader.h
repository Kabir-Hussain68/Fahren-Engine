#pragma once

#include <string>

class Shader
{
private:

public:
    virtual ~Shader() = default;

    virtual void bind() const = 0;
    virtual void unBind() const = 0;

    static Shader* create(const std::string& path);
    static Shader* create(const std::string& vertexSrx, const std::string& fragmentSrc);
};