#include "Application.h"
#include "Rendering_Vulkan.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <vector>
#include <fstream>

#ifndef NDEBUG
#define ENABLE_VALIDATION_LAYERS
#endif

Rendering_Vulkan Rendering_Vulkan_Init(SDL_Window* window)
{
    Rendering_Vulkan vulkan = {
        .instance = VK_NULL_HANDLE,
        .surface = VK_NULL_HANDLE,
        .device = VK_NULL_HANDLE,
        .swapchain = VK_NULL_HANDLE,
        .swapchainImages = nullptr,
        .swapchainImageViews = nullptr,
        .swapchainImageCount = 0
    };

    class Rendering_Vulkan_Scope
    {
    public:
        Rendering_Vulkan_Scope(Rendering_Vulkan* vulkan) : mVulkan(vulkan) {}
        ~Rendering_Vulkan_Scope()
        {
            if (mVulkan)
            {
                Rendering_Vulkan_Close(mVulkan);
            }
        }
        void commit() { mVulkan = nullptr; }
    private:
        Rendering_Vulkan* mVulkan;

    } scope(&vulkan);

    uint32_t enabledValidationLayersCount = 0;
    const char *const * enabledValidationLayers = nullptr;
#ifdef ENABLE_VALIDATION_LAYERS
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    std::vector<const char*> enabledValidationLayersVector;
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers)
    {
        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                enabledValidationLayersVector.push_back(layerName);
                break;
            }
        }
    }
    if (!enabledValidationLayersVector.empty())
    {
        enabledValidationLayers = enabledValidationLayersVector.data();
        enabledValidationLayersCount = enabledValidationLayersVector.size();
    }
#endif

    const VkApplicationInfo appCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = Application::name(),
        .applicationVersion = 0,
        .pEngineName = Application::name(),
        .engineVersion = 0,
        .apiVersion = VK_API_VERSION_1_0,
    };

    unsigned int sdlExtensionCount;
    SDL_bool sdlResult = SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, nullptr);
    if (sdlResult != SDL_TRUE) { return vulkan; }
    std::vector<const char*> sdlExtensions(sdlExtensionCount);
    sdlResult = SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, sdlExtensions.data());
    if (sdlResult != SDL_TRUE) { return vulkan; }

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .pApplicationInfo = &appCreateInfo,
        .enabledLayerCount = enabledValidationLayersCount,
        .ppEnabledLayerNames = enabledValidationLayers,
        .enabledExtensionCount = static_cast<uint32_t>(sdlExtensions.size()),
        .ppEnabledExtensionNames = (const char *const *) sdlExtensions.data(),
    };

    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &vulkan.instance);
    if (result != VK_SUCCESS) { return vulkan; }

    sdlResult = SDL_Vulkan_CreateSurface(window, vulkan.instance, &vulkan.surface);
    if (sdlResult != SDL_TRUE) { return vulkan; }

    uint32_t physicalDeviceCount = 0;
    result = vkEnumeratePhysicalDevices(vulkan.instance, &physicalDeviceCount, nullptr);
    if (result != VK_SUCCESS) { return vulkan; }
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    result = vkEnumeratePhysicalDevices(vulkan.instance, &physicalDeviceCount, physicalDevices.data());
    if (result != VK_SUCCESS) { return vulkan; }

    uint32_t queueFamilyPropertiesCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[0], &queueFamilyPropertiesCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[0], &queueFamilyPropertiesCount, queueFamilyProperties.data());
    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    uint32_t surfaceSupportedIndex = UINT32_MAX;
    for (uint32_t i = 0; i < queueFamilyPropertiesCount; i++)
    {
        if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            if (graphicsQueueNodeIndex == UINT32_MAX)
            {
                graphicsQueueNodeIndex = i;
            }
        }
        VkBool32 supported = VK_FALSE;
        result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], i, vulkan.surface, &supported);
        if (result != VK_SUCCESS) { return vulkan; }
        if (supported == VK_TRUE)
        {
            surfaceSupportedIndex = i;
            if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
            {
                graphicsQueueNodeIndex = i;
                break;
            }
        }
    }
    if (graphicsQueueNodeIndex == UINT32_MAX) { return vulkan; }

    uint32_t supportedExtensionsCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevices[0], nullptr, &supportedExtensionsCount, nullptr);
    std::vector<VkExtensionProperties> supportedExtensions;
    supportedExtensions.resize(supportedExtensionsCount);
    vkEnumerateDeviceExtensionProperties(physicalDevices[0], nullptr, &supportedExtensionsCount, supportedExtensions.data());
    std::vector<const char*> extensions;
    const char* knownExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    for (const char* knownExtension : knownExtensions)
    {
        for (const VkExtensionProperties& supportedExt : supportedExtensions)
        {
            if (SDL_strcmp(knownExtension, supportedExt.extensionName) == 0)
            {
                extensions.push_back(knownExtension);
            }
        }
    }

    if (extensions.empty()) { return vulkan; }

    uint32_t queueInfoCount = graphicsQueueNodeIndex == surfaceSupportedIndex ? 1 : 2;
    float queuePriorities =  0.0f;
    const VkDeviceQueueCreateInfo queueCreateInfo[2] {
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = queueInfoCount == 1 ? nullptr : &queueCreateInfo[1],
            .queueFamilyIndex = graphicsQueueNodeIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriorities
        },
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .queueFamilyIndex = surfaceSupportedIndex,
            .queueCount = 1,
            .pQueuePriorities = &queuePriorities
        }};
    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .queueCreateInfoCount = queueInfoCount,
        .pQueueCreateInfos = queueCreateInfo,
        .enabledLayerCount = enabledValidationLayersCount,
        .ppEnabledLayerNames = enabledValidationLayers,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = (const char *const *) extensions.data(),
        .pEnabledFeatures = nullptr
    };


    result = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &vulkan.device);
    if (result != VK_SUCCESS) { return vulkan; }

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevices[0], vulkan.surface, &surfaceCapabilities);
    VkExtent2D extents = surfaceCapabilities.currentExtent;

    if (extents.width == UINT32_MAX)
    {
        int sdlWidth;
        int sdlHeight;
        SDL_Vulkan_GetDrawableSize(window, &sdlWidth, &sdlHeight);

        extents.width = CLAMP(surfaceCapabilities.minImageExtent.width, (uint32_t)sdlWidth, surfaceCapabilities.maxImageExtent.width);
        extents.height = CLAMP(surfaceCapabilities.minImageExtent.height, (uint32_t)sdlHeight, surfaceCapabilities.maxImageExtent.height);
    }
    uint32_t surfaceImageCount = surfaceCapabilities.minImageCount;
    if (surfaceCapabilities.maxImageCount > 0 && surfaceImageCount > surfaceCapabilities.maxImageCount)
    {
        surfaceImageCount = surfaceCapabilities.maxImageCount;
    }

    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[0], vulkan.surface, &surfaceFormatCount, nullptr);
    if (surfaceFormatCount == 0) { return vulkan; }
    std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevices[0], vulkan.surface, &surfaceFormatCount, surfaceFormats.data());
    VkSurfaceFormatKHR chosenFormat = surfaceFormats[0];
    for (const auto& surfaceFormat : surfaceFormats)
    {
        if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            chosenFormat = surfaceFormat;
            break;
        }
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevices[0], vulkan.surface, &presentModeCount, nullptr);
    if (presentModeCount == 0) { return vulkan; }
    std::vector<VkPresentModeKHR> presentModes;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevices[0], vulkan.surface, &presentModeCount, presentModes.data());
    VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& presentMode : presentModes)
    {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            chosenPresentMode = presentMode;
        }
    }

    uint32_t queueFamilyIndices[] = { graphicsQueueNodeIndex, surfaceSupportedIndex };
    VkSwapchainCreateInfoKHR swapchainCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = vulkan.surface,
        .minImageCount = surfaceImageCount,
        .imageFormat = chosenFormat.format,
        .imageColorSpace = chosenFormat.colorSpace,
        .imageExtent = extents,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = queueInfoCount == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = queueInfoCount == 1 ? 0u : 2u,
        .pQueueFamilyIndices = queueInfoCount == 1 ? nullptr : queueFamilyIndices,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = chosenPresentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };
    result = vkCreateSwapchainKHR(vulkan.device, &swapchainCreateInfo, nullptr, &vulkan.swapchain);
    if (result != VK_SUCCESS) { return vulkan; }

    result = vkGetSwapchainImagesKHR(vulkan.device, vulkan.swapchain, &vulkan.swapchainImageCount, nullptr);
    if (result != VK_SUCCESS) { return vulkan; }
    vulkan.swapchainImages = (VkImage*)malloc(sizeof(VkImage) * vulkan.swapchainImageCount);
    vkGetSwapchainImagesKHR(vulkan.device, vulkan.swapchain, &vulkan.swapchainImageCount, vulkan.swapchainImages);

    vulkan.swapchainImageViews = (VkImageView*)malloc(sizeof(VkImageView) * vulkan.swapchainImageCount);
    SDL_memset(vulkan.swapchainImageViews, 0, sizeof(VkImageView) * vulkan.swapchainImageCount);

    for (uint32_t i = 0; i < vulkan.swapchainImageCount; ++i)
    {
        VkImageViewCreateInfo createInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = vulkan.swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = chosenFormat.format,
            .components = { .r = VK_COMPONENT_SWIZZLE_IDENTITY, .g = VK_COMPONENT_SWIZZLE_IDENTITY, .b = VK_COMPONENT_SWIZZLE_IDENTITY, .a = VK_COMPONENT_SWIZZLE_IDENTITY },
            .subresourceRange = { .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
        result = vkCreateImageView(vulkan.device, &createInfo, nullptr, &vulkan.swapchainImageViews[i]);
        if (result != VK_SUCCESS) { return vulkan; }
    }

    scope.commit();
    return vulkan;
}

Rendering_Vulkan_Shader::Rendering_Vulkan_Shader(VkDevice vkDevice, const char* vertexShaderFilename, const char* fragmentShaderFilename) :
    vertexShader(VK_NULL_HANDLE), fragmentShader(VK_NULL_HANDLE), device(vkDevice)
{
    auto readFile = [](const char* filename)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
        }
        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    };
    auto vertShaderCode = readFile(vertexShaderFilename);
    auto fragShaderCode = readFile(fragmentShaderFilename);

    VkShaderModuleCreateInfo vertexCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = vertShaderCode.size(),
        .pCode = reinterpret_cast<const uint32_t*>(vertShaderCode.data()),
    };
    VkResult result = vkCreateShaderModule(device, &vertexCreateInfo, nullptr, &vertexShader);
    if (result != VK_SUCCESS) { return; }

    VkShaderModuleCreateInfo fragmentCreateInfo {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = vertShaderCode.size(),
        .pCode = reinterpret_cast<const uint32_t*>(vertShaderCode.data()),
    };
    result = vkCreateShaderModule(device, &fragmentCreateInfo, nullptr, &fragmentShader);
    if (result != VK_SUCCESS) { return; }
}

Rendering_Vulkan_Shader::~Rendering_Vulkan_Shader()
{
    if (vertexShader == VK_NULL_HANDLE) { vkDestroyShaderModule(device, vertexShader, nullptr); }
    if (fragmentShader == VK_NULL_HANDLE) { vkDestroyShaderModule(device, fragmentShader, nullptr); }
    device = VK_NULL_HANDLE;
}

void Rendering_Vulkan_Close(Rendering_Vulkan* vulkan)
{
    if (vulkan->swapchainImageViews)
    {
        for (uint32_t i = 0; i < vulkan->swapchainImageCount; ++i)
        {
            if (vulkan->swapchainImageViews[i] != VK_NULL_HANDLE) { vkDestroyImageView(vulkan->device, vulkan->swapchainImageViews[i], nullptr); }
        }
        free(vulkan->swapchainImageViews);
        vulkan->swapchainImageViews = nullptr;
    }
    if (vulkan->swapchainImages != nullptr) { free(vulkan->swapchainImages); vulkan->swapchainImages = nullptr; }
    if (vulkan->swapchain != VK_NULL_HANDLE) { vkDestroySwapchainKHR(vulkan->device, vulkan->swapchain, nullptr); vulkan->swapchain = nullptr; }
    if (vulkan->device != VK_NULL_HANDLE) { vkDestroyDevice(vulkan->device, nullptr); vulkan->device = nullptr; }
    if (vulkan->surface != VK_NULL_HANDLE) { vkDestroySurfaceKHR(vulkan->instance, vulkan->surface, nullptr); vulkan->surface = nullptr; }
    if (vulkan->instance != VK_NULL_HANDLE) { vkDestroyInstance(vulkan->instance, nullptr); vulkan->instance = nullptr; }
}
