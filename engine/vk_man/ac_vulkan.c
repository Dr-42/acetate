#include "vk_man/ac_vulkan.h"

#include "core/ac_mem.h"

ac_vk_data* ac_vk_init(const char* app_name, bool enable_validation_layers, struct SDL_Window* window) {
    ac_vk_data* vk_data = ac_malloc(sizeof(ac_vk_data), AC_MEM_ENTRY_VULKAN);

    vk_data->device_data = init_vk_device(app_name, enable_validation_layers, window);
    vk_data->swapchain_data = init_vk_swapchain(&vk_data->device_data);

    return vk_data;
}

void ac_vk_cleanup(ac_vk_data* vk_data) {
    cleanup_vk_swapchain(&vk_data->swapchain_data, &vk_data->device_data);
    cleaup_vk_device(&vk_data->device_data);
    ac_free(vk_data);
}
