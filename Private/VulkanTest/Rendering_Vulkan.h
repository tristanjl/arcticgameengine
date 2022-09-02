#ifndef RENDERING_VULKAN_H
#define RENDERING_VULKAN_H

#include <vulkan/vulkan.h>

struct SDL_Window;

struct Rendering_Vulkan
{
    VkInstance instance;
    VkSurfaceKHR surface;
    VkDevice device;
    VkSwapchainKHR swapchain;
    VkImage* swapchainImages;
    VkImageView* swapchainImageViews;
    uint32_t swapchainImageCount;
};

class Rendering_Vulkan_Shader
{
public:
    Rendering_Vulkan_Shader(VkDevice vkDevice, const char* vertexShader, const char* fragmentShader);
    ~Rendering_Vulkan_Shader();

    VkShaderModule vertexShader;
    VkShaderModule fragmentShader;

private:
    VkDevice device;
};

Rendering_Vulkan Rendering_Vulkan_Init(SDL_Window* window);

void Rendering_Vulkan_Close(Rendering_Vulkan* vulkan);

#endif // RENDERING_VULKAN_H
