#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice;
class VulkanSwapChain;

class VulkanRenderPass
{
private:
    Ref<VulkanDevice> m_Device;
    Ref<VulkanSwapChain> m_SwapChain;

    VkRenderPass m_RenderPass;

    void createRenderPass();

public:
    VulkanRenderPass(Ref<VulkanDevice> device, Ref<VulkanSwapChain> swapChain);
    ~VulkanRenderPass();

    VkRenderPass getRenderPass() const { return m_RenderPass; }
};