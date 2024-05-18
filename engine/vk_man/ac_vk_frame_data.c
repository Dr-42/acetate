#include "vk_man/ac_vk_frame_data.h"

#include "vk_man/utils/ac_vk_command.h"
#include "vk_man/utils/ac_vk_sync_objs.h"

ac_vk_frame_data create_frame_data(VkDevice device, uint32_t queue_family_index) {
    ac_vk_frame_data frame_data = {0};
    frame_data.command_pool = create_command_pool(device, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, queue_family_index);
    frame_data.command_buffer = create_command_buffer(device, frame_data.command_pool);

    frame_data.swapchain_semaphore = create_semaphore(device, 0);
    frame_data.render_semaphore = create_semaphore(device, 0);

    frame_data.render_fence = create_fence(device, VK_FENCE_CREATE_SIGNALED_BIT);

    return frame_data;
}

void cleanup_frame_data(VkDevice device, ac_vk_frame_data* frame_data) {
    cleanup_fence(device, frame_data->render_fence);
    cleanup_semaphore(device, frame_data->render_semaphore);
    cleanup_semaphore(device, frame_data->swapchain_semaphore);
    cleanup_command_pool(device, frame_data->command_pool);
}
