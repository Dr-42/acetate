#include "vk_man/ac_vulkan.h"

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

#include "core/ac_log.h"
#include "ds/ac_string.h"
#include "core/ac_trace.h"
#include "ds/ac_darray.h"
#include "vk_man/utils/vk_init/ac_vk_init.h"

#include <stdlib.h>
#include <string.h>

#define VK_CHECK(x)                                           \
    do {                                                      \
        VkResult err = x;                                     \
        if (err) {                                            \
            const char* err_str = string_VkResult(err);       \
            ac_print_trace(3);                                \
            ac_log_fatal_exit("Vulkan error: %s\n", err_str); \
        }                                                     \
    } while (0)

static const char* validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
size_t validation_layers_count = sizeof(validation_layers) / sizeof(validation_layers[0]);

VkInstance create_instance(struct SDL_Window* window, const char* app_name, bool enable_validation_layers);
void init_vulkan(struct SDL_Window* window, bool enable_validation_layers, const char* app_name) {
    create_instance(window, app_name, enable_validation_layers);
}

VkInstance create_instance(struct SDL_Window* window, const char* app_name, bool enable_validation_layers) {
    if (enable_validation_layers) {
        if (!check_validation_layer_support(validation_layers, validation_layers_count)) {
            ac_log_fatal_exit("Validation layers requested, but not available!\n");
        }
    }
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = app_name;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Acetate Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    ac_darray_t* extensions = get_required_extensions(window, enable_validation_layers);
    char** extensions_arr = ac_malloc(sizeof(char*) * extensions->size, AC_MEM_ENTRY_VULKAN);
    ac_log_debug("SDL Vulkan extensions: \n");
    for (size_t i = 0; i < extensions->size; i++) {
        ac_string_t extension = {0};
        ac_darray_get(extensions, i, &extension);
        ac_log_debug("Extension: %s\n", extension.data);
        extensions_arr[i] = ac_malloc(extension.size + 1, AC_MEM_ENTRY_VULKAN);
        strcpy(extensions_arr[i], (const char*)extension.data);
    }

    createInfo.enabledExtensionCount = extensions->size;
    createInfo.ppEnabledExtensionNames = (const char**)extensions_arr;

    if (enable_validation_layers) {
        createInfo.enabledLayerCount = validation_layers_count;
        createInfo.ppEnabledLayerNames = validation_layers;
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = init_debug_messenger_create_info();
        createInfo.pNext = &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = NULL;
    }

    VkInstance instance;
    VkResult result = vkCreateInstance(&createInfo, NULL, &instance);
    VK_CHECK(result);

    for (size_t i = 0; i < extensions->size; i++) {
        ac_string_t extension = {0};
        ac_darray_get(extensions, i, &extension);
        ac_string_free(&extension);
        ac_free(extensions_arr[i]);
    }
    ac_free(extensions_arr);
    ac_darray_destroy(extensions);

    ac_log_info("Vulkan instance created\n");

    return instance;
}

void init_swapchain() {}
void init_command() {}
void init_sync() {}

ac_vk_data* ac_vk_init(const char* app_name, bool enable_validation_layers, struct SDL_Window* window) {
    ac_vk_data* vk_data = ac_malloc(sizeof(ac_vk_data), AC_MEM_ENTRY_VULKAN);
    vk_data->enable_validation_layers = enable_validation_layers;

    init_vulkan(window, enable_validation_layers, app_name);
    init_swapchain();
    init_command();
    init_sync();

    return vk_data;
}

void ac_vk_cleanup(ac_vk_data* vk_data) { ac_free(vk_data); }
