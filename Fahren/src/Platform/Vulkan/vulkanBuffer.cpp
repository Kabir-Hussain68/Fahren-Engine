#include "fhpch.h"
#include "vulkanBuffer.h"

#include "vulkanDevice.h"
#include "vulkanMemoryAllocator.h"

VulkanVertexBuffer::VulkanVertexBuffer(Ref<VulkanDevice> device, uint32_t size)
    : m_Device(device)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkResult result = vmaCreateBuffer(VulkanAllocator::getAllocator(), &bufferInfo, &allocInfo, &m_Buffer, &m_Allocation, nullptr);
    FH_CORE_ASSERT(result == VK_SUCCESS, "Failed to create dynamic vertex buffer!");
}

VulkanVertexBuffer::VulkanVertexBuffer(Ref<VulkanDevice> device, float* vertices, uint32_t size)
    : m_Device(device)
{
    VkBufferCreateInfo stagingInfo{};
    stagingInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingInfo.size = size;
    stagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo stagingAllocInfo{};
    stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkBuffer stagingBuffer;
    VmaAllocation stagingAllocation;
    VmaAllocationInfo stagingAllocResultInfo;
    vmaCreateBuffer(VulkanAllocator::getAllocator(), &stagingInfo, &stagingAllocInfo, &stagingBuffer, &stagingAllocation, &stagingAllocResultInfo);

    memcpy(stagingAllocResultInfo.pMappedData, vertices, size);

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

    VkResult result = vmaCreateBuffer(VulkanAllocator::getAllocator(), &bufferInfo, &allocInfo, &m_Buffer, &m_Allocation, nullptr);
    FH_CORE_ASSERT(result == VK_SUCCESS, "Failed to create static vertex buffer!");

    // TODO : record a one-time command buffer

    vmaDestroyBuffer(VulkanAllocator::getAllocator(), stagingBuffer, stagingAllocation);
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
    if (m_Buffer != VK_NULL_HANDLE)
        vmaDestroyBuffer(VulkanAllocator::getAllocator(), m_Buffer, m_Allocation);
}

void VulkanVertexBuffer::setData(const void* data, uint32_t size)
{
    VmaAllocationInfo allocInfo;
    vmaGetAllocationInfo(VulkanAllocator::getAllocator(), m_Allocation, &allocInfo);
    FH_CORE_ASSERT(allocInfo.pMappedData != nullptr, "Setting data on a non-mapped buffer!");
    memcpy(allocInfo.pMappedData, data, size);
}

VulkanIndexBuffer::VulkanIndexBuffer(Ref<VulkanDevice> device, uint32_t* indices, uint32_t count)
    : m_Device(device), m_Count(count)
{
    uint32_t size = count * sizeof(uint32_t);

    VkBufferCreateInfo stagingInfo{};
    stagingInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    stagingInfo.size = size;
    stagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VmaAllocationCreateInfo stagingAllocInfo{};
    stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
    stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

    VkBuffer stagingBuffer;
    VmaAllocation stagingAllocation;
    VmaAllocationInfo stagingResultInfo;
    vmaCreateBuffer(VulkanAllocator::getAllocator(), &stagingInfo, &stagingAllocInfo, &stagingBuffer, &stagingAllocation, &stagingResultInfo);
    memcpy(stagingResultInfo.pMappedData, indices, size);

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VmaAllocationCreateInfo allocInfo{};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

    VkResult result = vmaCreateBuffer(VulkanAllocator::getAllocator(), &bufferInfo, &allocInfo, &m_Buffer, &m_Allocation, nullptr);
    FH_CORE_ASSERT(result == VK_SUCCESS, "Failed to create index buffer!");

    vmaDestroyBuffer(VulkanAllocator::getAllocator(), stagingBuffer, stagingAllocation);
}

VulkanIndexBuffer::~VulkanIndexBuffer()
{
    if (m_Buffer != VK_NULL_HANDLE)
        vmaDestroyBuffer(VulkanAllocator::getAllocator(), m_Buffer, m_Allocation);
}