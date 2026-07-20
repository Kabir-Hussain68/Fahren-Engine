#pragma once

#include "Engine/renderer/buffer.h"

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

class VulkanDevice;

class VulkanVertexBuffer : public VertexBuffer
{
public:
    VulkanVertexBuffer(Ref<VulkanDevice> device, uint32_t size);
    VulkanVertexBuffer(Ref<VulkanDevice> device, float* vertices, uint32_t size);
    ~VulkanVertexBuffer() override;

    void bind() const override {}  
    void unBind() const override {} 

    void setData(const void* data, uint32_t size) override;

    void setLayout(const BufferLayout& layout) override { m_Layout = layout; }
    const BufferLayout& getLayout() const override { return m_Layout; }

    VkBuffer getVulkanHandle() const { return m_Buffer; }

private:
    Ref<VulkanDevice> m_Device;
    BufferLayout m_Layout;

    VkBuffer m_Buffer = VK_NULL_HANDLE;
    VmaAllocation m_Allocation = nullptr;
};

class VulkanIndexBuffer : public IndexBuffer
{
public:
    VulkanIndexBuffer(Ref<VulkanDevice> device, uint32_t* indices, uint32_t count);
    ~VulkanIndexBuffer() override;

    void bind() const override {}
    void unBind() const override {}

    uint32_t getCount() const override { return m_Count; }

    VkBuffer getVulkanHandle() const { return m_Buffer; }

private:
    Ref<VulkanDevice> m_Device;
    uint32_t m_Count;

    VkBuffer m_Buffer = VK_NULL_HANDLE;
    VmaAllocation m_Allocation = nullptr;
};