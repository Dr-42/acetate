#include "vk_man/ac_vulkan.h"
#include <time.h>
#include <vulkan/vulkan_core.h>

#include "core/ac_log.h"
#include "core/ac_mem.h"
#include "ds/ac_darray.h"

#include "math/ac_math_common.h"
#include "vk_man/ac_vk_frame_data.h"
#include "vk_man/ac_vk_swapchain.h"
#include "vk_man/utils/ac_vk_command.h"
#include "vk_man/utils/ac_vk_common.h"
#include "vk_man/utils/ac_vk_render.h"

ac_vk_data* ac_vk_init(const char* app_name, bool enable_validation_layers, struct SDL_Window* window) {
    ac_vk_data* vk_data = ac_malloc(sizeof(ac_vk_data), AC_MEM_ENTRY_VULKAN);

    vk_data->device_data = init_vk_device(app_name, enable_validation_layers, window);
    vk_data->swapchain_data = init_vk_swapchain(&vk_data->device_data);

    vk_data->frame_data =
        ac_darray_create(sizeof(ac_vk_frame_data), vk_data->swapchain_data.swapchain_image_count, AC_MEM_ENTRY_VULKAN);
    for (uint32_t i = 0; i < vk_data->swapchain_data.swapchain_image_count; i++) {
        ac_vk_frame_data frame_data = init_vk_frame_data(vk_data->device_data.device, vk_data->device_data.graphics_queue_idx);
        ac_darray_push(vk_data->frame_data, &frame_data);
        ac_log_debug("Frame data %d created\n", i);
    }
    vk_data->current_frame = 0;
    return vk_data;
}

void ac_vk_cleanup(ac_vk_data* vk_data) {
    ac_log_info("Cleaning up vulkan\n");
    ac_log_debug("Waiting for device to be idle\n");
    vkDeviceWaitIdle(vk_data->device_data.device);
    for (uint32_t i = 0; i < vk_data->frame_data->size; i++) {
        ac_vk_frame_data frame_data;
        ac_darray_get(vk_data->frame_data, i, &frame_data);
        cleanup_vk_frame_data(vk_data->device_data.device, &frame_data);
        ac_log_debug("Frame data %d destroyed\n", i);
    }
    ac_darray_destroy(vk_data->frame_data);
    cleanup_vk_swapchain(&vk_data->swapchain_data, &vk_data->device_data);
    cleaup_vk_device(&vk_data->device_data);
    ac_free(vk_data);
    ac_log_info("Vulkan cleaned up\n");
}

ac_vk_frame_data ac_vk_get_current_frame_data(ac_vk_data* vk_data) {
    ac_vk_frame_data frame_data;
    ac_darray_get(vk_data->frame_data, vk_data->current_frame, &frame_data);
    vk_data->current_frame = (vk_data->current_frame + 1) % vk_data->frame_data->size;
    return frame_data;
}

VkSemaphoreSubmitInfo semaphore_submit_info(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore) {
    VkSemaphoreSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
    submitInfo.pNext = VK_NULL_HANDLE;
    submitInfo.semaphore = semaphore;
    submitInfo.stageMask = stageMask;
    submitInfo.deviceIndex = 0;
    submitInfo.value = 1;

    return submitInfo;
}

VkCommandBufferSubmitInfo command_buffer_submit_info(VkCommandBuffer cmd) {
    VkCommandBufferSubmitInfo info = {0};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
    info.pNext = VK_NULL_HANDLE;
    info.commandBuffer = cmd;
    info.deviceMask = 0;

    return info;
}

VkSubmitInfo2 submit_info(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo,
                          VkSemaphoreSubmitInfo* waitSemaphoreInfo) {
    VkSubmitInfo2 info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
    info.pNext = VK_NULL_HANDLE;

    info.waitSemaphoreInfoCount = waitSemaphoreInfo == VK_NULL_HANDLE ? 0 : 1;
    info.pWaitSemaphoreInfos = waitSemaphoreInfo;

    info.signalSemaphoreInfoCount = signalSemaphoreInfo == VK_NULL_HANDLE ? 0 : 1;
    info.pSignalSemaphoreInfos = signalSemaphoreInfo;

    info.commandBufferInfoCount = 1;
    info.pCommandBufferInfos = cmd;

    return info;
}

void ac_vk_draw_frame(ac_vk_data* vk_data) {
    ac_vk_frame_data frame_d = ac_vk_get_current_frame_data(vk_data);
    ac_vk_frame_data* frame_data = &frame_d;
    VkResult res;

    res = vkWaitForFences(vk_data->device_data.device, 1, &frame_data->render_fence, VK_TRUE, UINT64_MAX);
    VK_CHECK(res);
    res = vkResetFences(vk_data->device_data.device, 1, &frame_data->render_fence);
    uint32_t image_index;

    res = vkAcquireNextImageKHR(vk_data->device_data.device, vk_data->swapchain_data.swapchain, UINT64_MAX,
                                frame_data->swapchain_semaphore, VK_NULL_HANDLE, &image_index);
    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        ac_log_warn("Swapchain out of date\n");
        recreate_vk_swapchain(&vk_data->swapchain_data, &vk_data->device_data);
        ac_log_info("Swapchain recreated\n");
        return;
    } else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
        VK_CHECK(res);
    }

    VkCommandBuffer cmd = frame_data->command_buffer;
    res = vkResetCommandBuffer(cmd, 0);
    VK_CHECK(res);

    begin_command_buffer(cmd, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    VkImage swapchain_image = {0};
    ac_darray_get(vk_data->swapchain_data.swapchain_images, image_index, &swapchain_image);
    transition_image(cmd, swapchain_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

    static size_t frame = 0;
    float g = (ac_math_sin(frame++ * 0.001f) + 1.0f) * 0.5f;

    VkClearColorValue clear_color = {{0.0f, g, 0.0f, 1.0f}};
    VkImageSubresourceRange clear_range = image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
    vkCmdClearColorImage(cmd, swapchain_image, VK_IMAGE_LAYOUT_GENERAL, &clear_color, 1, &clear_range);

    transition_image(cmd, swapchain_image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    end_command_buffer(cmd);

    VkCommandBufferSubmitInfo cmdinfo = command_buffer_submit_info(cmd);

    VkSemaphoreSubmitInfo waitInfo =
        semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, frame_data->swapchain_semaphore);

    VkSemaphoreSubmitInfo signalInfo = semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, frame_data->render_semaphore);

    VkSubmitInfo2 submit = submit_info(&cmdinfo, &signalInfo, &waitInfo);

    res = vkQueueSubmit2(vk_data->device_data.graphics_queue, 1, &submit, frame_data->render_fence);
    VK_CHECK(res);

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = VK_NULL_HANDLE;
    presentInfo.pSwapchains = &vk_data->swapchain_data.swapchain;
    presentInfo.swapchainCount = 1;

    presentInfo.pWaitSemaphores = &frame_data->render_semaphore;
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &image_index;

    res = vkQueuePresentKHR(vk_data->device_data.graphics_queue, &presentInfo);
    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        ac_log_warn("Swapchain out of date\n");
        recreate_vk_swapchain(&vk_data->swapchain_data, &vk_data->device_data);
        ac_log_info("Swapchain recreated\n");
        return;
    } else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
        VK_CHECK(res);
    }
    VK_CHECK(res);
}
