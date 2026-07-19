#pragma once

#include <vector>

#include <vulkan/vulkan.h>

class VulkanContext;
class VulkanDevice;

class VulkanSwapChain
{
private:
    Ref<VulkanContext> m_Context;
    Ref<VulkanDevice> m_Device;

    VkSwapchainKHR m_SwapChain;
    std::vector<VkImage> m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;
    VkFormat m_SwapChainImageFormat;
    VkExtent2D m_SwapChainExtent;

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

    void createSwapChain(uint32_t width, uint32_t height);
    void createImageViews();

public:
    VulkanSwapChain(Ref<VulkanContext> context, Ref<VulkanDevice> device, uint32_t width, uint32_t height);
    ~VulkanSwapChain();
};