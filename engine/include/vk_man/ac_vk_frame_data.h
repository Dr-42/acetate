#ifndef AC_VK_FRAME_DATA_H
#define AC_VK_FRAME_DATA_H

#include <vulkan/vulkan.h>

typedef struct ac_vk_frame_data {
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;
    VkSemaphore swapchain_semaphore;
    VkSemaphore render_semaphore;
    VkFence render_fence;
} ac_vk_frame_data;

ac_vk_frame_data create_frame_data(VkDevice device, uint32_t queue_family_index);
void cleanup_frame_data(VkDevice device, ac_vk_frame_data* frame_data);

#endif  // AC_VK_FRAME_DATA_H
