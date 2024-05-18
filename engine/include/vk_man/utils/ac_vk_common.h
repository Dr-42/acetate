#ifndef AC_VK_COMMON_H
#define AC_VK_COMMON_H

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include "core/ac_log.h"
#include "core/ac_trace.h"

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

#endif  // AC_VK_COMMON_H
