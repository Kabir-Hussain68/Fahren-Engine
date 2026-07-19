// vulkanShader.cpp
#include "fhpch.h"
#include "vulkanShader.h"
#include "vulkanDevice.h"

#include <fstream>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>

#include "Engine/core/timer.h"

namespace Utils
{
    static VkShaderStageFlagBits shaderTypeFromString(const std::string& type)
    {
        if (type == "vertex")
            return VK_SHADER_STAGE_VERTEX_BIT;
        if (type == "fragment" || type == "pixel")
            return VK_SHADER_STAGE_FRAGMENT_BIT;

        FH_CORE_ASSERT(false, "Unknown Shader type!");
        return (VkShaderStageFlagBits)0;
    }

    static shaderc_shader_kind vkShaderStageToShaderC(VkShaderStageFlagBits stage)
    {
        switch (stage)
        {
            case VK_SHADER_STAGE_VERTEX_BIT:
                return shaderc_glsl_vertex_shader;

            case VK_SHADER_STAGE_FRAGMENT_BIT:
                return shaderc_glsl_fragment_shader;
        }

        FH_CORE_ASSERT(false);
        return (shaderc_shader_kind)0;
    }

    static const char* vkShaderStageToString(VkShaderStageFlagBits stage)
    {
        switch (stage)
        {
            case VK_SHADER_STAGE_VERTEX_BIT:
                return "VK_SHADER_STAGE_VERTEX_BIT";

            case VK_SHADER_STAGE_FRAGMENT_BIT:
                return "VK_SHADER_STAGE_FRAGMENT_BIT";
        }

        FH_CORE_ASSERT(false);
        return nullptr;
    }

    static const char* getCacheDirectory()
    {
        return "assets/cache/shader/vulkan";
    }

    static void createCacheDirectoryIfNeeded()
    {
        std::string cacheDirectory = getCacheDirectory();
        if (!std::filesystem::exists(cacheDirectory))
        {
            std::filesystem::create_directories(cacheDirectory);
        }
    }

    static const char* vkShaderStageCachedFileExtension(VkShaderStageFlagBits stage)
    {
        switch (stage)
        {
            case VK_SHADER_STAGE_VERTEX_BIT:
                return ".cached_vulkan.vert";
            case VK_SHADER_STAGE_FRAGMENT_BIT:
                return ".cached_vulkan.frag";
        }

        FH_CORE_ASSERT(false);
        return "";
    }
}

VulkanShader::VulkanShader(Ref<VulkanDevice> device, const std::string& path)
    : m_Device(device), m_FilePath(path)
{
    FH_PROFILE_FUNCTION();

    Utils::createCacheDirectoryIfNeeded();

    std::string source = readFile(path);
    auto shaderSources = preProcess(source);

    {
        Timer timer;
        compileOrGetVulkanBinaries(shaderSources);
        createShaderModules();
        FH_CORE_WARN("Vulkan Shader Construction took {0} ms", timer.elapsedMillis());
    }

    auto lastSlash = path.find_last_of("/\\");
    lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
    auto lastDot = path.rfind('.');
    auto count = lastDot == std::string::npos ? path.size() - lastSlash : lastDot - lastSlash;
    m_Name = path.substr(lastSlash, count);
}

VulkanShader::VulkanShader(Ref<VulkanDevice> device, const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
    : m_Device(device), m_Name(name)
{
    FH_PROFILE_FUNCTION();

    std::unordered_map<VkShaderStageFlagBits, std::string> sources;
    sources[VK_SHADER_STAGE_VERTEX_BIT] = vertexSrc;
    sources[VK_SHADER_STAGE_FRAGMENT_BIT] = fragmentSrc;

    compileOrGetVulkanBinaries(sources);
    createShaderModules();
}

VulkanShader::~VulkanShader()
{
    FH_PROFILE_FUNCTION();

    VkDevice vkDevice = m_Device->getDevice();
    for (auto&& [stage, module] : m_ShaderModules)
    {
        if (module != VK_NULL_HANDLE)
            vkDestroyShaderModule(vkDevice, module, nullptr);
    }
}

std::string VulkanShader::readFile(const std::string& path)
{
    FH_PROFILE_FUNCTION();

    std::string result;
    std::ifstream in(path, std::ios::in | std::ios::binary);
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

std::unordered_map<VkShaderStageFlagBits, std::string> VulkanShader::preProcess(const std::string& source)
{
    FH_PROFILE_FUNCTION();

    std::unordered_map<VkShaderStageFlagBits, std::string> shaderSources;

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

void VulkanShader::compileOrGetVulkanBinaries(const std::unordered_map<VkShaderStageFlagBits, std::string>& shaderSources)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
    const bool optimize = true;
    if (optimize)
    {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }

    std::filesystem::path cacheDirectory = Utils::getCacheDirectory();

    m_VulkanSPIRV.clear();
    for (auto&& [stage, source] : shaderSources)
    {
        std::filesystem::path shaderFilePath = m_FilePath;
        std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::vkShaderStageCachedFileExtension(stage));

        std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
        if (in.is_open())
        {
            in.seekg(0, std::ios::end);
            auto size = in.tellg();
            in.seekg(0, std::ios::beg);

            auto& data = m_VulkanSPIRV[stage];
            data.resize(size / sizeof(uint32_t));
            in.read((char*)data.data(), size);
        }
        else
        {
            shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::vkShaderStageToShaderC(stage), m_FilePath.c_str(), options);
            if (module.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                FH_CORE_ERROR(module.GetErrorMessage());
                FH_CORE_ASSERT(false);
            }

            m_VulkanSPIRV[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

            std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
            if (out.is_open())
            {
                auto& data = m_VulkanSPIRV[stage];
                out.write((char*)data.data(), data.size() * sizeof(uint32_t));
                out.flush();
                out.close();
            }
        }
    }

    m_ReflectedUniformBuffers.clear();
    for (auto&& [stage, data] : m_VulkanSPIRV)
        reflect(stage, data);
}

void VulkanShader::createShaderModules()
{
    VkDevice vkDevice = m_Device->getDevice();

    m_ShaderModules.clear();
    for (auto&& [stage, spirvData] : m_VulkanSPIRV)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = spirvData.size() * sizeof(uint32_t);
        createInfo.pCode = spirvData.data();

        VkShaderModule module = VK_NULL_HANDLE;
        VkResult result = vkCreateShaderModule(vkDevice, &createInfo, nullptr, &module);
        FH_CORE_ASSERT(result == VK_SUCCESS, "Failed to create shader module!");

        m_ShaderModules[stage] = module;
    }
}

void VulkanShader::reflect(VkShaderStageFlagBits stage, const std::vector<uint32_t>& shaderData)
{
    spirv_cross::Compiler compiler(shaderData);
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    FH_CORE_TRACE("VulkanShader::Reflect - {0} {1}", Utils::vkShaderStageToString(stage), m_FilePath);
    FH_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
    FH_CORE_TRACE("    {0} resources", resources.sampled_images.size());

    FH_CORE_TRACE("Uniform buffers:");
    for (const auto& resource : resources.uniform_buffers)
    {
        const auto& bufferType = compiler.get_type(resource.base_type_id);
        uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
        uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
        uint32_t memberCount = static_cast<uint32_t>(bufferType.member_types.size());

        FH_CORE_TRACE("  {0}", resource.name);
        FH_CORE_TRACE("    Size = {0}", bufferSize);
        FH_CORE_TRACE("    Binding = {0}", binding);
        FH_CORE_TRACE("    Members = {0}", memberCount);

        ReflectedUniformBuffer reflected;
        reflected.name = resource.name;
        reflected.binding = binding;
        reflected.size = bufferSize;
        reflected.memberCount = memberCount;
        reflected.stage = stage;
        m_ReflectedUniformBuffers.push_back(reflected);
    }
}