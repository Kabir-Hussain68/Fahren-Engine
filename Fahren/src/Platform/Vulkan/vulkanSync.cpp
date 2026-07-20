#include "fhpch.h"
#include "vulkanSync.h"

#include "vulkanDevice.h"

VulkanSync::VulkanSync(Ref<VulkanDevice> device, uint32_t frameIndex)
    : m_Device(device)
{
    createSyncObjects(frameIndex);
}

VulkanSync::~VulkanSync()
{
    VkDevice device = m_Device->getDevice();
    for (size_t i = 0; i < m_ImageAvailableSemaphores.size(); i++)
    {
        vkDestroySemaphore(device, m_ImageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(device, m_RenderFinishedSemaphores[i], nullptr);
        vkDestroyFence(device, m_InFlightFences[i], nullptr);
    }
}

void VulkanSync::createSyncObjects(uint32_t frameIndex)
{
    VkDevice device = m_Device->getDevice();

    m_ImageAvailableSemaphores.resize(frameIndex);
    m_RenderFinishedSemaphores.resize(frameIndex);
    m_InFlightFences.resize(frameIndex);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < frameIndex; i++)
    {
        VkResult result1 = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]);
        FH_CORE_ASSERT(result1 == VK_SUCCESS, "Failed to create image Semaphores");
        VkResult result2 = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]);
        FH_CORE_ASSERT(result2 == VK_SUCCESS, "Failed to create avialable Semaphores");
        VkResult result3 = vkCreateFence(device, &fenceInfo, nullptr, &m_InFlightFences[i]);
        FH_CORE_ASSERT(result3 == VK_SUCCESS, "Failed to create fences");
    }
}

void VulkanSync::waitForFence(uint32_t frameIndex)
{
    VkDevice device = m_Device->getDevice();
    vkWaitForFences(device, 1, &m_InFlightFences[frameIndex], VK_TRUE, UINT64_MAX);
}

void VulkanSync::resetFence(uint32_t frameIndex)
{
    VkDevice device = m_Device->getDevice();
    vkResetFences(device, 1, &m_InFlightFences[frameIndex]);
}
