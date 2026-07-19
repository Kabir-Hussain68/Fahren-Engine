#include "fhpch.h"
#include "vulkanFrameBuffer.h"

#include "vulkanDevice.h"
#include "vulkanSwapChain.h"
#include "vulkanRenderPass.h"

VulkanFrameBuffer::VulkanFrameBuffer(Ref<VulkanDevice> device, Ref<VulkanSwapChain> swapChain, Ref<VulkanRenderPass> renderPass)
    : m_Device(device), m_SwapChain(swapChain), m_RenderPass(renderPass)
{
    createFrameBuffers();
}

VulkanFrameBuffer::~VulkanFrameBuffer()
{
    VkDevice device = m_Device->getDevice();
    for (auto framebuffer : m_SwapChainFrameBuffers) 
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
}

void VulkanFrameBuffer::createFrameBuffers()
{
    VkDevice device = m_Device->getDevice();

    VkExtent2D swapChainExtent = m_SwapChain->getSwapChainExtent();
    auto swapChainImageViews = m_SwapChain->getSwapChainImageViews();
    size_t swapChainImageViewsCount = swapChainImageViews.size();

    VkRenderPass renderPass = m_RenderPass->getRenderPass();

    m_SwapChainFrameBuffers.resize(swapChainImageViewsCount);
    for (size_t i = 0; i < swapChainImageViewsCount; i++)
    {
        VkImageView attachments[] = { swapChainImageViews[i] };

        VkFramebufferCreateInfo frameBufferInfo{};
        frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferInfo.renderPass = renderPass;
        frameBufferInfo.attachmentCount = 1;
        frameBufferInfo.pAttachments = attachments;
        frameBufferInfo.width = swapChainExtent.width;
        frameBufferInfo.height = swapChainExtent.height;
        frameBufferInfo.layers = 1;
    
        VkResult result = vkCreateFramebuffer(device, &frameBufferInfo, nullptr, &m_SwapChainFrameBuffers[i]);
        FH_CORE_ASSERT(result == VK_SUCCESS, "Failed to create framebuffer");
    }

}
