#pragma once

#include <optional>
#include <vulkan/vulkan.h>

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class VulkanContext;

class VulkanDevice
{
private:
    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_Device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    QueueFamilyIndices m_QueueFamilyIndices;
    Ref<VulkanContext> m_Context;

    void pickPhysicalDevice();
    void createLogicalDevice();

    int rateDeviceSuitability(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

public:
    VulkanDevice(Ref<VulkanContext> context);
    ~VulkanDevice();

    VkPhysicalDevice getPhysicalDevice() const { return m_PhysicalDevice; }
    VkDevice getDevice() const { return m_Device; }
    uint32_t getGraphicsQueueFamily() { return m_QueueFamilyIndices.graphicsFamily.value(); } 
    uint32_t getPresentQueueFamily() { return m_QueueFamilyIndices.presentFamily.value(); } 
};