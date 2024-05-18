#ifndef AC_VK_RENDER_H
#define AC_VK_RENDER_H

#include <vulkan/vulkan.h>

VkImageView create_image_view(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);

#endif  // AC_VK_RENDER_H
