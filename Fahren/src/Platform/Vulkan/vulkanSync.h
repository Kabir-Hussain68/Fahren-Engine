#pragma once

#include <vulkan/vulkan.h>

class VulkanDevice;

class VulkanSync
{
private:   
    Ref<VulkanDevice> m_Device;

    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;

    void createSyncObjects(uint32_t frameIndex);
    void waitForFence(uint32_t frameIndex);
    void resetFence(uint32_t frameIndex);

public:
    VulkanSync(Ref<VulkanDevice> device, uint32_t frameIndex);
    ~VulkanSync();

    VkSemaphore getImageSemaphone(uint32_t frameIndex) const { return m_ImageAvailableSemaphores[frameIndex]; }
    VkSemaphore getFinishedSemaphone(uint32_t frameIndex) const { return m_RenderFinishedSemaphores[frameIndex]; }
    VkFence getInFlightFence(uint32_t frameIndex) const { return m_InFlightFences[frameIndex]; }
};