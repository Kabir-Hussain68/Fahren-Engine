#pragma once

#include "Engine/renderer/shader.h"

#include <vulkan/vulkan.h>

struct ReflectedUniformBuffer
{
    std::string name;
    uint32_t binding;
    uint32_t size;
    uint32_t memberCount;
    VkShaderStageFlagBits stage;
};

class VulkanDevice;

class VulkanShader : public Shader
{
private:
    Ref<VulkanDevice> m_Device;

    std::string m_FilePath;
    std::string m_Name;

    std::unordered_map<VkShaderStageFlagBits, std::vector<uint32_t>> m_VulkanSPIRV;
    std::unordered_map<VkShaderStageFlagBits, VkShaderModule> m_ShaderModules;

    std::vector<ReflectedUniformBuffer> m_ReflectedUniformBuffers;

    std::string readFile(const std::string& path);
    std::unordered_map<VkShaderStageFlagBits, std::string> preProcess(const std::string& source);

    void compileOrGetVulkanBinaries(const std::unordered_map<VkShaderStageFlagBits, std::string>& shaderSources);
    void createShaderModules();
    void reflect(VkShaderStageFlagBits stage, const std::vector<uint32_t>& shaderData);

public:
    VulkanShader(Ref<VulkanDevice> device, const std::string& path);
    VulkanShader(Ref<VulkanDevice> device, const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
    ~VulkanShader() override;

    const std::string& getName() const { return m_Name; }
    const std::vector<ReflectedUniformBuffer>& getReflectedUniformBuffers() const { return m_ReflectedUniformBuffers; }

    virtual void bind() const override {}
    virtual void unBind() const override {}

    virtual void setMat4(const std::string& name, const glm::mat4& value) override {}
    virtual void setInt(const std::string& name, int value) override {}
    virtual void setIntArray(const std::string& name, int* values, uint32_t count) override {}
    virtual void setFloat(const std::string& name, float value) override {}
    virtual void setFloat2(const std::string& name, const glm::vec2& value) override {}
    virtual void setFloat3(const std::string& name, const glm::vec3& value) override {}
    virtual void setFloat4(const std::string& name, const glm::vec4& value) override {}
};