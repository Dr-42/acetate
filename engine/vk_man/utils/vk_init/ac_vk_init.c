#include "vk_man/utils/vk_init/ac_vk_init.h"

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_vulkan.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "core/ac_mem.h"
#include "core/ac_log.h"

#include "ds/ac_darray.h"
#include "ds/ac_string.h"

#include <string.h>
#include <vulkan/vulkan_core.h>

bool check_validation_layer_support(const char** validation_layers, size_t validation_layers_count) {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    ac_darray_t* available_layers = ac_darray_create(sizeof(VkLayerProperties), layer_count, AC_MEM_ENTRY_VULKAN);

    vkEnumerateInstanceLayerProperties(&layer_count, available_layers->data);
    available_layers->size = layer_count;

    for (size_t i = 0; i < validation_layers_count; i++) {
        bool layer_found = false;
        const char* layer_name = validation_layers[i];
        for (size_t j = 0; j < available_layers->size; j++) {
            VkLayerProperties layer_properties = ((VkLayerProperties*)available_layers->data)[j];
            ac_log_debug("Available layer: %s\n", layer_properties.layerName);
            if (strcmp(layer_name, layer_properties.layerName) == 0) {
                layer_found = true;
                break;
            }
        }
        if (!layer_found) {
            ac_log_warn("Validation layer %s not found\n", layer_name);
            ac_darray_destroy(available_layers);
            return false;
        }
    }
    ac_log_info("Validation layers supported\n");
    ac_darray_destroy(available_layers);
    return true;
}

ac_darray_t* get_required_extensions(SDL_Window* window, bool enable_validation_layers) {
    uint32_t SDL_extension_count = 0;
    bool got_count = SDL_Vulkan_GetInstanceExtensions(window, &SDL_extension_count, NULL);
    if (!got_count) {
        ac_log_fatal_exit("Failed to get SDL Vulkan extensions count\n");
    }
    const char** extensions_arr = ac_malloc(sizeof(char*) * SDL_extension_count, AC_MEM_ENTRY_VULKAN);
    bool got_extensions = SDL_Vulkan_GetInstanceExtensions(window, &SDL_extension_count, extensions_arr);
    ac_darray_t* extensions = ac_darray_create(sizeof(ac_string_t), SDL_extension_count, AC_MEM_ENTRY_VULKAN);

    for (size_t i = 0; i < SDL_extension_count; i++) {
        ac_string_t extension = ac_string_new_from_str(extensions_arr[i], AC_MEM_ENTRY_VULKAN);
        ac_darray_push(extensions, &extension);
    }

    if (!got_extensions) {
        ac_log_fatal_exit("Failed to get SDL Vulkan extensions\n");
    }
    if (enable_validation_layers) {
        ac_string_t debug_extension = ac_string_new_from_str(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, AC_MEM_ENTRY_VULKAN);
        ac_darray_push(extensions, &debug_extension);
    }
    ac_free(extensions_arr);
    ac_log_info("Got required extensions for SDL\n");
    return extensions;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    (void)messageType;
    (void)pUserData;
    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            ac_log_trace("Validation layer: %s\n", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            ac_log_info("Validation layer: %s\n", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            ac_log_warn("Validation layer: %s\n", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            ac_log_error("Validation layer: %s\n", pCallbackData->pMessage);
            break;
        default:
            ac_log_warn("Validation layer: %s\n", pCallbackData->pMessage);
            break;
    }
    return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT* init_debug_messenger_create_info() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = NULL;
    VkDebugUtilsMessengerCreateInfoEXT* createInfoPtr = ac_malloc(sizeof(VkDebugUtilsMessengerCreateInfoEXT), AC_MEM_ENTRY_VULKAN);
    memcpy(createInfoPtr, &createInfo, sizeof(VkDebugUtilsMessengerCreateInfoEXT));
    return createInfoPtr;
}

void find_queue_families(VkPhysicalDevice* physical_device, queue_family_indices_t* indices, VkSurfaceKHR* surface) {
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(*physical_device, &queue_family_count, NULL);
    ac_darray_t* queue_families = ac_darray_create(sizeof(VkQueueFamilyProperties), queue_family_count, AC_MEM_ENTRY_VULKAN);
    vkGetPhysicalDeviceQueueFamilyProperties(*physical_device, &queue_family_count, queue_families->data);
    queue_families->size = queue_family_count;
    for (uint32_t i = 0; i < queue_family_count; i++) {
        VkQueueFamilyProperties queue_properties = {0};
        ac_darray_get(queue_families, i, &queue_properties);
        if (queue_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices->graphics_family = i;
            indices->found_graphics_family = true;
        }
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(*physical_device, i, *surface, &present_support);
        if (present_support) {
            indices->present_family = i;
            indices->found_present_family = true;
        }
        if (queue_properties.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            indices->transfer_family = i;
            indices->found_transfer_family = true;
        }
        if (indices->found_graphics_family && indices->found_present_family && indices->found_transfer_family) {
            break;
        }
    }
    ac_darray_destroy(queue_families);
}

bool check_device_extension_support(VkPhysicalDevice device, const char** required_extensions, size_t required_extensions_count) {
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, NULL);
    ac_darray_t* available_extensions = ac_darray_create(sizeof(VkExtensionProperties), extension_count, AC_MEM_ENTRY_VULKAN);
    vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, available_extensions->data);
    available_extensions->size = extension_count;

    for (size_t i = 0; i < required_extensions_count; i++) {
        ac_log_info("Checking for extension: %s\n", required_extensions[i]);
        bool extension_found = false;
        const char* required_extension = required_extensions[i];
        for (size_t j = 0; j < available_extensions->size; j++) {
            VkExtensionProperties extension_properties = ((VkExtensionProperties*)available_extensions->data)[j];
            if (strcmp(required_extension, extension_properties.extensionName) == 0) {
                extension_found = true;
                break;
            }
        }
        if (!extension_found) {
            ac_darray_destroy(available_extensions);
            return false;
        }
    }
    ac_darray_destroy(available_extensions);
    return true;
}

bool check_swapchain_adequete(VkPhysicalDevice device, VkSurfaceKHR* surface) {
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, *surface, &capabilities);
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, *surface, &format_count, NULL);
    ac_darray_t* formats = ac_darray_create(sizeof(VkSurfaceFormatKHR), format_count, AC_MEM_ENTRY_VULKAN);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, *surface, &format_count, formats->data);
    formats->size = format_count;
    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, *surface, &present_mode_count, NULL);
    ac_darray_t* present_modes = ac_darray_create(sizeof(VkPresentModeKHR), present_mode_count, AC_MEM_ENTRY_VULKAN);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, *surface, &present_mode_count, present_modes->data);
    present_modes->size = present_mode_count;
    if (formats->size == 0 || present_modes->size == 0) {
        ac_darray_destroy(formats);
        ac_darray_destroy(present_modes);
        return false;
    }
    ac_darray_destroy(formats);
    ac_darray_destroy(present_modes);
    return true;
}
