#ifndef AC_VULKAN_H
#define AC_VULKAN_H

#include <stdbool.h>

#include "ds/ac_darray.h"

#include "vk_man/ac_vk_device.h"
#include "vk_man/ac_vk_swapchain.h"
#include "vk_man/ac_vk_frame_data.h"

struct SDL_Window;

/// Vulkan data
/// Contains all vulkan data
/// Fields:
/// - device_data: Vulkan device data
/// - swapchain_data: Vulkan swapchain data
/// - frame_data: Vector of ac_vk_frame_data
/// - current_frame: Index of current frame
/// Methods:
/// - ac_vk_init: Initializes vulkan
/// - ac_vk_get_current_frame_data: Returns current frame data
/// - ac_vk_draw_frame: Draws a frame
/// - ac_vk_cleanup: Cleans up vulkan
typedef struct ac_vk_data {
    ac_vk_device_data device_data;
    ac_vk_swapchain_data swapchain_data;
    ac_darray_t* frame_data;  // Vector of ac_vk_frame_data
    size_t current_frame;
} ac_vk_data;

ac_vk_data* ac_vk_init(const char* app_name, bool enable_validation_layers, struct SDL_Window* window);
ac_vk_frame_data ac_vk_get_current_frame_data(ac_vk_data* vk_data);
void ac_vk_draw_frame(ac_vk_data* vk_data);
void ac_vk_cleanup(ac_vk_data* vk_data);

#endif  // AC_VULKAN_H
