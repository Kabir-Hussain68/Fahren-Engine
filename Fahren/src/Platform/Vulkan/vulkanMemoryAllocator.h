#pragma once
#include <vk_mem_alloc.h>

class VulkanDevice;
class VulkanContext;

namespace VulkanAllocator
{
    void init(Ref<VulkanContext> context, Ref<VulkanDevice> device);
    void shutdown();
    VmaAllocator getAllocator();
};