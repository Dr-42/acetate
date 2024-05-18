#ifndef AC_VK_SYNC_OBJS_H
#define AC_VK_SYNC_OBJS_H

#include <vulkan/vulkan.h>

VkSemaphore create_semaphore(VkDevice device, VkSemaphoreCreateFlags flags);
VkFence create_fence(VkDevice device, VkFenceCreateFlags flags);

#endif  // AC_VK_SYNC_OBJS_H
