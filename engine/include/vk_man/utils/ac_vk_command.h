#ifndef AC_VK_COMMAND_H
#define AC_VK_COMMAND_H

#include <vulkan/vulkan.h>

VkCommandPool create_command_pool(VkDevice device, uint32_t queue_family_index);
VkCommandBuffer create_command_buffer(VkDevice device, VkCommandPool command_pool);

void cleanup_command_pool(VkDevice device, VkCommandPool command_pool);

#endif  // AC_VK_COMMAND_H
