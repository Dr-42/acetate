#include "vk_man/ac_vulkan.h"

#include "core/ac_log.h"
#include "core/ac_mem.h"

#include "vk_man/ac_vk_frame_data.h"

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

    return vk_data;
}

void ac_vk_cleanup(ac_vk_data* vk_data) {
    for (uint32_t i = 0; i < vk_data->frame_data->size; i++) {
        ac_vk_frame_data frame_data;
        ac_darray_get(vk_data->frame_data, i, &frame_data);
        cleanup_vk_frame_data(vk_data->device_data.device, &frame_data);
    }
    ac_darray_destroy(vk_data->frame_data);
    cleanup_vk_swapchain(&vk_data->swapchain_data, &vk_data->device_data);
    cleaup_vk_device(&vk_data->device_data);
    ac_free(vk_data);
}
