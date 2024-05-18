#ifndef AC_VK_SYNC_OBJS_H
#define AC_VK_SYNC_OBJS_H

#include <vulkan/vulkan.h>

VkSemaphore create_semaphore(VkDevice device, VkSemaphoreCreateFlags flags);
VkFence create_fence(VkDevice device, VkFenceCreateFlags flags);

void cleanup_semaphore(VkDevice device, VkSemaphore semaphore);
void cleanup_fence(VkDevice device, VkFence fence);

#endif  // AC_VK_SYNC_OBJS_H
