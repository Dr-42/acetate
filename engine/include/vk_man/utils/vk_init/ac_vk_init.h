#ifndef AC_VK_UTILS_INIT_H
#define AC_VK_UTILS_INIT_H

#include <vulkan/vulkan.h>

#include "ds/ac_darray.h"

#include <stdbool.h>

struct SDL_Window;

bool check_validation_layer_support(const char** validation_layers, size_t validation_layers_count);
ac_darray_t* get_required_extensions(struct SDL_Window* window, bool enable_validation_layers);
VkDebugUtilsMessengerCreateInfoEXT init_debug_messenger_create_info();
#endif  // AC_VK_UTILS_INIT_H
