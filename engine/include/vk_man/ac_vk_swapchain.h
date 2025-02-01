#ifndef AC_VK_SWAPCHAIN_H
#define AC_VK_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include "ds/ac_darray.h"
#include "vk_man/ac_vk_device.h"

typedef struct ac_vk_swapchain_data {
    VkSwapchainKHR swapchain;
    VkFormat swapchain_image_format;
    uint32_t swapchain_image_count;
    ac_darray_t* swapchain_images;
    ac_darray_t* swapchain_image_views;
    VkExtent2D swapchain_extent;
} ac_vk_swapchain_data;

ac_vk_swapchain_data init_vk_swapchain(ac_vk_device_data* vk_device_data);
void recreate_vk_swapchain(ac_vk_swapchain_data* vk_swapchain_data, ac_vk_device_data* vk_device_data);
void cleanup_vk_swapchain(ac_vk_swapchain_data* vk_swapchain_data, ac_vk_device_data* vk_device_data);

#endif  // AC_VK_SWAPCHAIN_H
