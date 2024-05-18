#ifndef AC_VULKAN_H
#define AC_VULKAN_H

#include <stdbool.h>

#include "ds/ac_darray.h"

#include "vk_man/ac_vk_device.h"
#include "vk_man/ac_vk_swapchain.h"

struct SDL_Window;

typedef struct ac_vk_data {
    ac_vk_device_data device_data;
    ac_vk_swapchain_data swapchain_data;
    ac_darray_t* frame_data;  // Vector of ac_vk_frame_data
} ac_vk_data;

ac_vk_data* ac_vk_init(const char* app_name, bool enable_validation_layers, struct SDL_Window* window);
void ac_vk_cleanup(ac_vk_data* vk_data);

#endif  // AC_VULKAN_H
