#ifndef AC_VULKAN_H
#define AC_VULKAN_H

#include <vulkan/vulkan.h>
#include <stdbool.h>

struct SDL_Window;

typedef struct ac_vk_data {
    bool enable_validation_layers;
    struct SDL_Window* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT* debug_messenger;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkSurfaceKHR surface;
} ac_vk_data;

ac_vk_data* ac_vk_init(const char* app_name, bool enable_validation_layers, struct SDL_Window* window);
void ac_vk_cleanup(ac_vk_data* vk_data);

#endif  // AC_VULKAN_H
