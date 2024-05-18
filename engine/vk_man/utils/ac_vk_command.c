#include "vk_man/utils/ac_vk_command.h"

#include "vk_man/utils/ac_vk_common.h"

VkCommandPool create_command_pool(VkDevice device, VkCommandPoolCreateFlags flags, uint32_t queue_family_index) {
    VkCommandPoolCreateInfo pool_info = {0};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = queue_family_index;
    pool_info.flags = flags;

    VkCommandPool command_pool;
    VkResult res = vkCreateCommandPool(device, &pool_info, NULL, &command_pool);

    VK_CHECK(res);

    return command_pool;
}

VkCommandBuffer create_command_buffer(VkDevice device, VkCommandPool command_pool) {
    VkCommandBufferAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    VkResult res = vkAllocateCommandBuffers(device, &alloc_info, &command_buffer);

    VK_CHECK(res);

    return command_buffer;
}

void cleanup_command_pool(VkDevice device, VkCommandPool command_pool) { vkDestroyCommandPool(device, command_pool, NULL); }
