#pragma once

#include "Engine/renderer/graphicsContext.h"

struct GLFWwindow;
struct VkInstance_T;
struct VkSurfaceKHR_T;
struct VkDebugUtilsMessengerEXT_T;

using VkInstance = VkInstance_T*;
using VkSurfaceKHR = VkSurfaceKHR_T*;
using VkDebugUtilsMessengerEXT = VkDebugUtilsMessengerEXT_T*;

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
    virtual ~VulkanContext();

    virtual void Init() override;

    VkInstance getVulkanInstance() { return m_Instance; }
    VkSurfaceKHR getVulkanSurface() { return m_Surface; }
};