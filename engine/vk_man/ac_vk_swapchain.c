#include "vk_man/ac_vk_swapchain.h"

#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>
#include <SDL2/SDL.h>

#include "core/ac_mem.h"
#include "core/ac_log.h"
#include "core/ac_trace.h"
#include "ds/ac_darray.h"
#include "math/ac_math_common.h"
#include "vk_man/utils/ac_vk_init.h"
#include "vk_man/utils/ac_vk_render.h"

#include <stdlib.h>

#define VK_CHECK(x)                                           \
    do {                                                      \
        VkResult err = x;                                     \
        if (err) {                                            \
            const char* err_str = string_VkResult(err);       \
            ac_print_trace(3);                                \
            ac_log_fatal_exit("Vulkan error: %s\n", err_str); \
        }                                                     \
    } while (0)

typedef struct ac_vk_swapchain_support_details {
    VkSurfaceCapabilitiesKHR capabilities;
    ac_darray_t* formats;
    ac_darray_t* present_modes;
} ac_vk_swapchain_support_details;

static ac_vk_swapchain_support_details query_swapchain_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
    ac_vk_swapchain_support_details details = {0};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details.capabilities);

    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, NULL);

    if (format_count != 0) {
        details.formats = ac_darray_create(sizeof(VkSurfaceFormatKHR), format_count, AC_MEM_ENTRY_VULKAN);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, details.formats->data);
        details.formats->size = format_count;
    }

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, NULL);

    if (present_mode_count != 0) {
        details.present_modes = ac_darray_create(sizeof(VkPresentModeKHR), present_mode_count, AC_MEM_ENTRY_VULKAN);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, details.present_modes->data);
        details.present_modes->size = present_mode_count;
    }

    return details;
}

VkSurfaceFormatKHR choose_swap_surface_format(ac_darray_t* formats) {
    VkSurfaceFormatKHR format = {0};
    for (uint32_t i = 0; i < formats->size; i++) {
        ac_darray_get(formats, i, &format);
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }
    ac_darray_get(formats, 0, &format);
    return format;
}

VkPresentModeKHR choose_swap_present_mode(ac_darray_t* present_modes) {
    VkPresentModeKHR present_mode = {};
    for (uint32_t i = 0; i < present_modes->size; i++) {
        ac_darray_get(present_modes, i, &present_mode);
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return present_mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR* capabilities, SDL_Window* window) {
    if (capabilities->currentExtent.width != UINT32_MAX) {
        return capabilities->currentExtent;
    } else {
        VkExtent2D actual_extent = {0};
        int width = 0;
        int height = 0;
        SDL_GetWindowSize(window, &width, &height);
        actual_extent.width = ac_math_clamp(width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
        actual_extent.height = ac_math_clamp(height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);
        return actual_extent;
    }
}

ac_darray_t* create_swapchain_images(VkDevice* device, ac_darray_t* swap_chain_images, VkFormat swap_chainformat) {
    uint32_t swap_chain_images_count = swap_chain_images->size;
    ac_darray_t* swapchain_image_views = ac_darray_create(sizeof(VkImageView), swap_chain_images_count, AC_MEM_ENTRY_VULKAN);
    for (uint32_t i = 0; i < swap_chain_images_count; i++) {
        VkImageView image_view =
            create_image_view(*device, ((VkImage*)swap_chain_images->data)[i], swap_chainformat, VK_IMAGE_ASPECT_COLOR_BIT);
        ac_darray_push(swapchain_image_views, &image_view);
        ac_log_debug("Swapchain image view created for index %d\n", i);
    }
    return swapchain_image_views;
}

ac_vk_swapchain_data init_vk_swapchain(ac_vk_device_data* vk_device_data) {
    ac_vk_swapchain_data swapchain_data = {0};
    ac_vk_swapchain_support_details swapchain_support_data =
        query_swapchain_support(vk_device_data->physical_device, vk_device_data->surface);

    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(swapchain_support_data.formats);
    VkPresentModeKHR present_mode = choose_swap_present_mode(swapchain_support_data.present_modes);
    VkExtent2D extent = choose_swap_extent(&swapchain_support_data.capabilities, vk_device_data->window);

    uint32_t image_count = swapchain_support_data.capabilities.minImageCount + 1;
    if (swapchain_support_data.capabilities.maxImageCount > 0 && image_count > swapchain_support_data.capabilities.maxImageCount) {
        image_count = swapchain_support_data.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = vk_device_data->surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    queue_family_indices_t indicies = {0};
    find_queue_families(&vk_device_data->physical_device, &indicies, &vk_device_data->surface);

    if (indicies.graphics_family != indicies.present_family) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = (uint32_t[]){indicies.graphics_family, indicies.present_family};
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = NULL;
    }

    create_info.preTransform = swapchain_support_data.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(vk_device_data->device, &create_info, NULL, &swapchain_data.swapchain);
    VK_CHECK(result);

    ac_darray_destroy(swapchain_support_data.formats);
    ac_darray_destroy(swapchain_support_data.present_modes);

    ac_log_info("Swapchain created\n");

    uint32_t swapchain_image_count = 0;
    vkGetSwapchainImagesKHR(vk_device_data->device, swapchain_data.swapchain, &swapchain_image_count, NULL);
    swapchain_data.swapchainImages = ac_darray_create(sizeof(VkImage), swapchain_image_count, AC_MEM_ENTRY_VULKAN);
    vkGetSwapchainImagesKHR(vk_device_data->device, swapchain_data.swapchain, &swapchain_image_count,
                            swapchain_data.swapchainImages->data);
    swapchain_data.swapchainImages->size = swapchain_image_count;

    swapchain_data.swapchainImageFormat = surface_format.format;
    swapchain_data.swapchainExtent = extent;

    swapchain_data.swapchainImageViews =
        create_swapchain_images(&vk_device_data->device, swapchain_data.swapchainImages, swapchain_data.swapchainImageFormat);

    return swapchain_data;
}

void cleanup_vk_swapchain(ac_vk_swapchain_data* vk_swapchain_data, ac_vk_device_data* vk_device_data) {
    size_t swapchain_image_count = vk_swapchain_data->swapchainImages->size;
    for (size_t i = 0; i < swapchain_image_count; i++) {
        VkImageView image_view = {};
        ac_darray_get(vk_swapchain_data->swapchainImageViews, i, &image_view);
        vkDestroyImageView(vk_device_data->device, image_view, NULL);
    }
    ac_darray_destroy(vk_swapchain_data->swapchainImageViews);
    ac_darray_destroy(vk_swapchain_data->swapchainImages);
    vkDestroySwapchainKHR(vk_device_data->device, vk_swapchain_data->swapchain, NULL);
}
