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
    std::vector<VkCommandBuffer> m_CommandBuffers;

    void createCommandPool();
    void createCommandBuffer(uint32_t frameIndex);

    void recordCommandBuffer(uint32_t frameIndex, VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent);
    void endRecording(uint32_t frameIndex);

public:
    VulkanCommandBuffer(Ref<VulkanDevice> device, uint32_t frameIndex);
    ~VulkanCommandBuffer();

    VkCommandBuffer getCommandBuffer(uint32_t frameIndex) const { return m_CommandBuffers[frameIndex]; }
};