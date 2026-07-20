#include "fhpch.h"
#include "vulkanCommandBuffer.h"

#include "vulkanDevice.h"


VulkanCommandBuffer::VulkanCommandBuffer(Ref<VulkanDevice> device, uint32_t frameIndex)
    : m_Device(device)
{
    createCommandPool();
    createCommandBuffer(frameIndex);
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    VkDevice device = m_Device->getDevice();
    vkDestroyCommandPool(device, m_CommandPool, nullptr);
}

void VulkanCommandBuffer::createCommandPool()
{
    VkDevice device = m_Device->getDevice();

    uint32_t graphicsFamily = m_Device->getGraphicsQueueFamily();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = graphicsFamily;

    VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &m_CommandPool);
    FH_CORE_ASSERT(result == VK_SUCCESS, "Failed to create command pool");
}

void VulkanCommandBuffer::createCommandBuffer(uint32_t frameIndex)
{
    VkDevice device = m_Device->getDevice();

    m_CommandBuffers.resize(frameIndex);
    
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

    VkResult result = vkAllocateCommandBuffers(device, &allocInfo, m_CommandBuffers.data());
    FH_CORE_ASSERT(result == VK_SUCCESS, "Failed to create command buffers");
}

void VulkanCommandBuffer::recordCommandBuffer(uint32_t frameIndex, VkRenderPass renderPass, VkFramebuffer framebuffer, VkExtent2D extent)
{
    VkCommandBuffer commandBuffer = m_CommandBuffers[frameIndex];
    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;

    VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    FH_CORE_ASSERT(result == VK_SUCCESS, "Failed to begin recording command buffer");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = extent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::endRecording(uint32_t frameIndex)
{
    VkCommandBuffer commandBuffer = m_CommandBuffers[frameIndex];

    vkCmdEndRenderPass(commandBuffer);

    VkResult result = vkEndCommandBuffer(commandBuffer);
    FH_CORE_ASSERT(result == VK_SUCCESS, "Failed to record command buffer!");
}
