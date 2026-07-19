#pragma once

#include "Engine/renderer/graphicsContext.h"

#include <vulkan/vulkan.h>

struct GLFWwindow;

#ifdef FH_ENABLE_ASSERTS
    bool enableValidationLayers = true;
#else
    bool enableValidationLayers = false;
#endif

static const std::vector<const char*> s_ValidationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

class VulkanContext : public GraphicsContext
{
private:
    GLFWwindow* m_WindowHandle;
    VkInstance m_Instance;
    VkSurfaceKHR m_Surface;
    VkDebugUtilsMessengerEXT m_DebugMessenger;

    void createInstance();
    void setupDebugMessenger();
    void createSurface();

public:
    VulkanContext(GLFWwindow* windowHandle);
    ~VulkanContext();

    virtual void Init() override;

    VkInstance getVulkanInstance() { return m_Instance; }
    VkSurfaceKHR getVulkanSurface() { return m_Surface; }
};