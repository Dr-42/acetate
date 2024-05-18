#include "vk_man/utils/ac_vk_sync_objs.h"

#include "vk_man/utils/ac_vk_common.h"

VkSemaphore create_semaphore(VkDevice device, VkSemaphoreCreateFlags flags) {
    VkSemaphoreCreateInfo semaphore_info = {0};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_info.flags = flags;

    VkSemaphore semaphore;
    VkResult res = vkCreateSemaphore(device, &semaphore_info, NULL, &semaphore);

    VK_CHECK(res);

    return semaphore;
}

VkFence create_fence(VkDevice device, VkFenceCreateFlags flags) {
    VkFenceCreateInfo fence_info = {0};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = flags;

    VkFence fence;
    VkResult res = vkCreateFence(device, &fence_info, NULL, &fence);

    VK_CHECK(res);

    return fence;
}

void cleanup_semaphore(VkDevice device, VkSemaphore semaphore) { vkDestroySemaphore(device, semaphore, NULL); }

void cleanup_fence(VkDevice device, VkFence fence) { vkDestroyFence(device, fence, NULL); }
