#pragma once

#include <optional>

struct VkPhysicalDevice_T;
struct VkDevice_T;
struct VkQueue_T;

using VkPhysicalDevice = VkPhysicalDevice_T*;
using VkDevice = VkDevice_T*;
using VkQueue = VkQueue_T*;

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
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    Ref<VulkanContext> m_Context;

    void pickPhysicalDevice();
    void createLogicalDevice();

    int rateDeviceSuitability(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

public:
    VulkanDevice(Ref<VulkanContext> context);
    virtual ~VulkanDevice();
};