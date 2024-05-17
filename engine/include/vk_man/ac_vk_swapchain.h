#ifndef AC_VK_SWAPCHAIN_H
#define AC_VK_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include "ds/ac_darray.h"
#include "vk_man/ac_vk_device.h"

typedef struct ac_vk_swapchain_data {
    VkSwapchainKHR swapchain;
    VkFormat swapchainImageFormat;
    ac_darray_t* swapchainImages;
    ac_darray_t* swapchainImageViews;
    VkExtent2D swapchainExtent;
} ac_vk_swapchain_data;

ac_vk_swapchain_data init_vk_swapchain(ac_vk_device_data* vk_device_data);
void cleanup_vk_swapchain(ac_vk_swapchain_data* vk_swapchain_data, ac_vk_device_data* vk_device_data);

#endif  // AC_VK_SWAPCHAIN_H
