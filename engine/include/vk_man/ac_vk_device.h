#ifndef AC_VK_DEVICE_H
#define AC_VK_DEVICE_H

#include <vulkan/vulkan.h>
#include <stdbool.h>

typedef struct ac_vk_device_data {
    bool enable_validation_layers;
    struct SDL_Window* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT* debug_messenger;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkSurfaceKHR surface;
    VkQueue graphics_queue;
    uint32_t graphics_queue_idx;
} ac_vk_device_data;

ac_vk_device_data init_vk_device(const char* app_name, bool enable_validation_layers, struct SDL_Window* window);

VkInstance create_instance(const char* app_name, ac_vk_device_data* vk_device_data);
VkSurfaceKHR create_surface(ac_vk_device_data* vk_device_data);
void setup_debug_messenger(ac_vk_device_data* vk_device_data);
void create_physical_device(ac_vk_device_data* vk_device_data);
void create_logical_device(ac_vk_device_data* vk_device_data);

void cleaup_vk_device(ac_vk_device_data* vk_device_data);

#endif  // AC_VK_DEVICE_H
