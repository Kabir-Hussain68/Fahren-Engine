#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice;
class VulkanSwapChain;
class VulkanRenderPass;

class VulkanFrameBuffer
{
private:
    Ref<VulkanDevice> m_Device;
    Ref<VulkanSwapChain> m_SwapChain;
    Ref<VulkanRenderPass> m_RenderPass;

    std::vector<VkFramebuffer> m_SwapChainFrameBuffers;

    void createFrameBuffers();

public:
    VulkanFrameBuffer(Ref<VulkanDevice> device, Ref<VulkanSwapChain> swapChain, Ref<VulkanRenderPass> renderPass );
    ~VulkanFrameBuffer();

    std::vector<VkFramebuffer> getSwapChainFrameBuffers() { return m_SwapChainFrameBuffers; }
};