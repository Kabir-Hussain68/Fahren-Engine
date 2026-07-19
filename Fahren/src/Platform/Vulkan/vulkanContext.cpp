#include "fhpch.h"
#include "vulkanContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

VulkanContext::VulkanContext(GLFWwindow *windowHandle)
    : m_WindowHandle(windowHandle)
{
    FH_CORE_ASSERT(m_WindowHandle, "Window Handle is null");
}

VulkanContext::~VulkanContext()
{
    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    m_Surface = VK_NULL_HANDLE;

    if (enableValidationLayers && m_DebugMessenger != VK_NULL_HANDLE)
    {
        auto destroyDebugUtilsMessengerFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");

        if (destroyDebugUtilsMessengerFunc)
            destroyDebugUtilsMessengerFunc(m_Instance, m_DebugMessenger, nullptr);

        m_DebugMessenger = VK_NULL_HANDLE;
    }

    vkDestroyInstance(m_Instance, nullptr);
    m_Instance = VK_NULL_HANDLE;
}

void VulkanContext::Init()
{
    createInstance();
    setupDebugMessenger();
    createSurface();
}

static bool checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : s_ValidationLayers)
    {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;
}

void VulkanContext::createInstance()
{
    if (enableValidationLayers)
    {
        FH_CORE_ASSERT(checkValidationLayerSupport(), "Validation layers requested, but not available!");
    }

    // Creates the application Info
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = "Fahren";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0);
    applicationInfo.pEngineName = "Fahren Engine";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    // Getting all the extensions and checking if they are valid or not
    uint32_t instanceExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&instanceExtensionCount);
    FH_CORE_ASSERT(glfwExtensions != nullptr, "GLFW returned no required instance extensions - was glfwInit() called?");

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + instanceExtensionCount);
    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    // Creating the Instance with the extensions and the application info
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = s_ValidationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
    FH_CORE_ASSERT(result == VK_SUCCESS, "Could not create Vulkan Instance!");
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        FH_CORE_WARN("Vulkan Validation: {0}", pCallbackData->pMessage);
    else
        FH_CORE_TRACE("Vulkan Validation: {0}", pCallbackData->pMessage);

    return VK_FALSE;
}

void VulkanContext::setupDebugMessenger()
{
    if (!enableValidationLayers) 
        return;
        
    VkDebugUtilsMessengerCreateInfoEXT createDebugInfo{};
    createDebugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createDebugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createDebugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                            | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    createDebugInfo.pfnUserCallback = debugCallback;
    createDebugInfo.pUserData = nullptr;

    auto createDebugUtilsMessengerFunc = (PFN_vkCreateDebugUtilsMessengerEXT)
    vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");

    FH_CORE_ASSERT(createDebugUtilsMessengerFunc != nullptr, "Could not load vkCreateDebugUtilsMessengerEXT");

    VkResult result = createDebugUtilsMessengerFunc(m_Instance, &createDebugInfo, nullptr, &m_DebugMessenger);
    FH_CORE_ASSERT(result == VK_SUCCESS, "Failed to set up debug messenger!");
}

void VulkanContext::createSurface()
{
    VkResult result = glfwCreateWindowSurface(m_Instance, m_WindowHandle, nullptr, &m_Surface);
    FH_CORE_ASSERT(result == VK_SUCCESS, "Failed to create window surface");
}
