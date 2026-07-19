#include "fhpch.h"
#include "vulkanDevice.h"
#include "vulkanContext.h"
#include "vulkanUtils.h"

#include <map>
#include <set>

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VulkanDevice::VulkanDevice(Ref<VulkanContext> context)
    : m_Context(context)
{
    pickPhysicalDevice();
    createLogicalDevice();
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyDevice(m_Device, nullptr);
}

void VulkanDevice::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    VkInstance instance = m_Context->getVulkanInstance();
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    FH_CORE_ASSERT(deviceCount != 0, "Failed to find GPU with Vulkan support!");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Enumerating a map to select a gpu with the higest score (GPUScore > CPUScore)
    std::multimap<int, VkPhysicalDevice> candidates;

    for (auto& device : devices)
    {
        int score = rateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    if (candidates.rbegin()->first > 0)
        m_PhysicalDevice = candidates.rbegin()->second;
    else
        FH_CORE_ASSERT(false, "Failed to find usable GPU");
}

void VulkanDevice::createLogicalDevice()
{
    m_QueueFamilyIndices = findQueueFamilies(m_PhysicalDevice);

    float queuePriority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {m_QueueFamilyIndices.graphicsFamily.value(), m_QueueFamilyIndices.presentFamily.value()};

    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = s_ValidationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);
    FH_CORE_ASSERT(result == VK_SUCCESS, "Could not create logical device!");

    vkGetDeviceQueue(m_Device, m_QueueFamilyIndices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(m_Device, m_QueueFamilyIndices.presentFamily.value(), 0, &presentQueue);
}

int VulkanDevice::rateDeviceSuitability(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = findQueueFamilies(device);
    if (!indices.isComplete())
        return 0;

    bool extensionSupported = checkDeviceExtensionSupport(device);
    if (!extensionSupported)
        return 0;

    bool swapChainAdequete = false;
    if (extensionSupported)
    {
        SwapChainSupportDetails swapChainSupport = VulkanUtils::querySwapChainSupport(device, m_Context->getVulkanSurface());
        swapChainAdequete = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    if (!swapChainAdequete)
        return 0;

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int score = 0;

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D;

    if (!deviceFeatures.geometryShader)
    {
        return 0;
    }

    return score;
}

bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;
    VkSurfaceKHR surface = m_Context->getVulkanSurface();

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        if (presentSupport)
        {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) 
        {
            break;
        }

        i++;
    }

    return indices;
}
