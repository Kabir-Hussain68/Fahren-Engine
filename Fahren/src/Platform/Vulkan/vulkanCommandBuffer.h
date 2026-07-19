#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice;
class VulkanRenderPass;
class VulkanFrameBuffer;
class VulkanSwapChain;

class VulkanCommandBuffer
{
private:
    Ref<VulkanDevice> m_Device;
    VkCommandPool m_CommandPool;
    VkCommandBuffer m_CommandBuffer;

    void createCommandPool();
    void createCommandBuffer();

    void recordCommandBuffer(VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent, uint32_t imageIndex);
    void endRecording();

public:
    VulkanCommandBuffer(Ref<VulkanDevice> device);
    ~VulkanCommandBuffer();

    VkCommandBuffer getCommandBuffer() { return m_CommandBuffer; }
};