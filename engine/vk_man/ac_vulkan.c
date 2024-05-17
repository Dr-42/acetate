#include "vk_man/ac_vulkan.h"

#include <stdint.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_vulkan.h>

#include "core/ac_log.h"
#include "ds/ac_string.h"
#include "core/ac_trace.h"
#include "ds/ac_darray.h"
#include "vk_man/utils/vk_init/ac_vk_init.h"

#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

#define VK_CHECK(x)                                           \
    do {                                                      \
        VkResult err = x;                                     \
        if (err) {                                            \
            const char* err_str = string_VkResult(err);       \
            ac_print_trace(3);                                \
            ac_log_fatal_exit("Vulkan error: %s\n", err_str); \
        }                                                     \
    } while (0)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static const char* validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
static const char* device_extensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

VkInstance create_instance(const char* app_name, ac_vk_data* vk_data);
VkSurfaceKHR create_surface(ac_vk_data* vk_data);
void setup_debug_messenger(ac_vk_data* vk_data);
void create_physical_device(ac_vk_data* vk_data);
void create_logical_device(ac_vk_data* vk_data);

void init_vulkan(const char* app_name, ac_vk_data* vk_data) {
    vk_data->instance = create_instance(app_name, vk_data);
    vk_data->surface = create_surface(vk_data);
    setup_debug_messenger(vk_data);
    create_physical_device(vk_data);
    create_logical_device(vk_data);
}

VkInstance create_instance(const char* app_name, ac_vk_data* vk_data) {
    if (vk_data->enable_validation_layers) {
        if (!check_validation_layer_support(validation_layers, ARRAY_SIZE(validation_layers))) {
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

    ac_darray_t* extensions = get_required_extensions(vk_data->window, vk_data->enable_validation_layers);
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
    VkDebugUtilsMessengerCreateInfoEXT* debugCreateInfo = NULL;
    if (vk_data->enable_validation_layers) {
        createInfo.enabledLayerCount = ARRAY_SIZE(validation_layers);
        createInfo.ppEnabledLayerNames = validation_layers;
        debugCreateInfo = init_debug_messenger_create_info();
        createInfo.pNext = debugCreateInfo;
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
    ac_free(debugCreateInfo);

    ac_log_info("Vulkan instance created\n");

    return instance;
}

VkSurfaceKHR create_surface(ac_vk_data* vk_data) {
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(vk_data->window, vk_data->instance, &surface)) {
        ac_log_fatal_exit("Failed to create SDL Vulkan surface\n");
    }
    ac_log_info("SDL Vulkan surface created\n");
    return surface;
}

void setup_debug_messenger(ac_vk_data* vk_data) {
    if (!vk_data->enable_validation_layers) {
        return;
    }
    VkDebugUtilsMessengerCreateInfoEXT* createInfo = init_debug_messenger_create_info();
    PFN_vkCreateDebugUtilsMessengerEXT func =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vk_data->instance, "vkCreateDebugUtilsMessengerEXT");
    if (func == NULL) {
        ac_log_fatal_exit("Failed to get vkCreateDebugUtilsMessengerEXT function\n");
    }
    vk_data->debug_messenger = ac_malloc(sizeof(VkDebugUtilsMessengerEXT), AC_MEM_ENTRY_VULKAN);
    VkResult result = func(vk_data->instance, createInfo, NULL, vk_data->debug_messenger);
    VK_CHECK(result);
    ac_log_info("Debug messenger created\n");
    ac_free(createInfo);
}

void create_physical_device(ac_vk_data* vk_data) {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(vk_data->instance, &device_count, NULL);
    if (device_count == 0) {
        ac_log_fatal_exit("Failed to find GPUs with Vulkan support\n");
    }

    ac_darray_t* devices = ac_darray_create(sizeof(VkPhysicalDevice), device_count, AC_MEM_ENTRY_VULKAN);
    vkEnumeratePhysicalDevices(vk_data->instance, &device_count, (VkPhysicalDevice*)devices->data);
    devices->size = device_count;
    for (size_t i = 0; i < devices->size; i++) {
        VkPhysicalDevice device;
        ac_darray_get(devices, i, &device);
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;

        vkGetPhysicalDeviceProperties(device, &properties);
        vkGetPhysicalDeviceFeatures(device, &features);

        ac_log_info("Checking device: %s\n", properties.deviceName);

        if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            ac_log_debug("Device is not a discrete GPU\n");
            continue;
        }

        if (!features.samplerAnisotropy) {
            ac_log_debug("Device does not support geometry shaders\n");
            continue;
        }

        queue_family_indices_t indicies = {0};
        find_queue_families(&device, &indicies, &vk_data->surface);

        if (!indicies.found_graphics_family || !indicies.found_present_family || !indicies.found_transfer_family) {
            ac_log_debug("Device does not support required queue families\n");
            continue;
        }

        bool extensions_supported = check_device_extension_support(device, device_extensions, ARRAY_SIZE(device_extensions));
        if (!extensions_supported) {
            ac_log_debug("Device does not support required extensions\n");
            continue;
        }

        bool swapchain_adequate = check_swapchain_adequete(device, &vk_data->surface);
        if (!swapchain_adequate) {
            ac_log_debug("Device does not support required swapchain\n");
            continue;
        }

        ac_log_info("Suitable Device found: %s\n", properties.deviceName);
        vk_data->physical_device = device;
        ac_darray_destroy(devices);
        return;
    }
    ac_log_fatal_exit("Failed to find suitable GPU\n");
}

void create_logical_device(ac_vk_data* vk_data) {
    queue_family_indices_t indicies = {0};
    find_queue_families(&vk_data->physical_device, &indicies, &vk_data->surface);

    ac_darray_t* unique_queue_families = ac_darray_create(sizeof(uint32_t), 3, AC_MEM_ENTRY_VULKAN);
    uint32_t queue_families[] = {indicies.graphics_family, indicies.present_family, indicies.transfer_family};
    for (size_t i = 0; i < ARRAY_SIZE(queue_families); i++) {
        bool found = false;
        for (size_t j = 0; j < unique_queue_families->size; j++) {
            uint32_t queue_family;
            ac_darray_get(unique_queue_families, j, &queue_family);
            if (queue_families[i] == queue_family) {
                found = true;
                continue;
            }
        }
        if (!found) {
            ac_darray_push(unique_queue_families, &queue_families[i]);
        }
    }
    ac_darray_t* queue_create_infos =
        ac_darray_create(sizeof(VkDeviceQueueCreateInfo), unique_queue_families->size, AC_MEM_ENTRY_VULKAN);
    for (size_t i = 0; i < unique_queue_families->size; i++) {
        VkDeviceQueueCreateInfo queueCreateInfo = {0};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queue_families[i];
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        ac_darray_push(queue_create_infos, &queueCreateInfo);
    }

    ac_log_debug("Unique queue families: %zu\n", unique_queue_families->size);

    VkPhysicalDeviceFeatures device_features = {0};
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = (VkDeviceQueueCreateInfo*)queue_create_infos->data;
    create_info.queueCreateInfoCount = queue_create_infos->size;
    create_info.pEnabledFeatures = &device_features;

    if (!check_device_extension_support(vk_data->physical_device, device_extensions, ARRAY_SIZE(device_extensions))) {
        ac_log_fatal_exit("Device does not support required extensions\n");
    }
    ac_log_info("Required device extensions supported\n");

    create_info.enabledExtensionCount = ARRAY_SIZE(device_extensions);
    create_info.ppEnabledExtensionNames = device_extensions;

    if (vk_data->enable_validation_layers) {
        create_info.enabledLayerCount = ARRAY_SIZE(validation_layers);
        create_info.ppEnabledLayerNames = validation_layers;
    } else {
        create_info.enabledLayerCount = 0;
    }

    VkResult result = vkCreateDevice(vk_data->physical_device, &create_info, NULL, &vk_data->device);
    VK_CHECK(result);
    ac_log_info("Logical device created\n");
    ac_darray_destroy(queue_create_infos);
    ac_darray_destroy(unique_queue_families);
}

void init_swapchain() {}
void init_command() {}
void init_sync() {}

ac_vk_data* ac_vk_init(const char* app_name, bool enable_validation_layers, struct SDL_Window* window) {
    ac_vk_data* vk_data = ac_malloc(sizeof(ac_vk_data), AC_MEM_ENTRY_VULKAN);
    vk_data->enable_validation_layers = enable_validation_layers;
    vk_data->window = window;

    init_vulkan(app_name, vk_data);
    init_swapchain();
    init_command();
    init_sync();

    return vk_data;
}

void destroy_debug_utils_messenger(VkInstance instance, VkDebugUtilsMessengerEXT* debug_messenger,
                                   const VkAllocationCallbacks* pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, *debug_messenger, pAllocator);
    }
}

void ac_vk_cleanup(ac_vk_data* vk_data) {
    if (vk_data->enable_validation_layers) {
        destroy_debug_utils_messenger(vk_data->instance, vk_data->debug_messenger, NULL);
    }
    vkDestroyDevice(vk_data->device, NULL);
    vkDestroySurfaceKHR(vk_data->instance, vk_data->surface, NULL);
    vkDestroyInstance(vk_data->instance, NULL);
    ac_free(vk_data->debug_messenger);
    ac_free(vk_data);
}
