#pragma once

#include <string>
#include <unordered_map>

class Shader
{
private:

public:
    virtual ~Shader() = default;

    virtual void bind() const = 0;
    virtual void unBind() const = 0;

    virtual const std::string& getName() const = 0;

    static Ref<Shader> create(const std::string& path);
    static Ref<Shader> create(const std::string& name, const std::string& vertexSrx, const std::string& fragmentSrc);
};

class ShaderLibrary
{
private:
    std::unordered_map<std::string, Ref<Shader>> m_Shaders;

public:
    void add(const std::string& name, const Ref<Shader>& shader);
    void add(const Ref<Shader>& shader);
    Ref<Shader> load(const std::string& path);
    Ref<Shader> load(const std::string& name, const std::string& path);

    Ref<Shader> get(const std::string& name);

    bool exists(const std::string& name) const;
};