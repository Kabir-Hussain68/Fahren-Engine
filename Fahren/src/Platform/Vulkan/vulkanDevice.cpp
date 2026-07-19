#include "fhpch.h"
#include "vulkanDevice.h"
#include "vulkanContext.h"

#include <map>
#include <set>

#include <vulkan/vulkan.h>

VulkanDevice::VulkanDevice(Ref<VulkanContext> context)
    : m_Context(context)
{
    pickPhysicalDevice();
    createLogicalDevice();
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyDevice(device, nullptr);
}

void VulkanDevice::pickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    VkInstance m_Instance = m_Context->getVulkanInstance();
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

    FH_CORE_ASSERT(deviceCount != 0, "Failed to find GPU with Vulkan support!");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

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
    QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);

    float queuePriority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

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
    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = s_ValidationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &device);
    FH_CORE_ASSERT(result == VK_SUCCESS, "Could not create logical device!");

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

int VulkanDevice::rateDeviceSuitability(VkPhysicalDevice device)
{
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

QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;
    VkSurfaceKHR m_Surface = m_Context->getVulkanSurface();

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

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
