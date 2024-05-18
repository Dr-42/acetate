#ifndef AC_VK_UTILS_INIT_H
#define AC_VK_UTILS_INIT_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "ds/ac_darray.h"

#include <stdbool.h>

struct SDL_Window;

typedef struct queue_family_indices_t {
    uint32_t graphics_family;
    bool found_graphics_family;
    uint32_t present_family;
    bool found_present_family;
    uint32_t transfer_family;
    bool found_transfer_family;
} queue_family_indices_t;

bool check_validation_layer_support(const char** validation_layers, size_t validation_layers_count);
ac_darray_t* get_required_extensions(struct SDL_Window* window, bool enable_validation_layers);
VkDebugUtilsMessengerCreateInfoEXT* init_debug_messenger_create_info();
void find_queue_families(VkPhysicalDevice* physical_device, queue_family_indices_t* indices, VkSurfaceKHR* surface);
bool check_device_extension_support(VkPhysicalDevice device, const char** required_extensions, size_t required_extensions_count);
bool check_swapchain_adequete(VkPhysicalDevice device, VkSurfaceKHR* surface);

#endif  // AC_VK_UTILS_INIT_H
