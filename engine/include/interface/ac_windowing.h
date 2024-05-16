#ifndef AC_WINDOWING_H
#define AC_WINDOWING_H

/**
 * @file ac_windowing.h
 * @brief This file contains the interface for the windowing system.
 */

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

/**
 * @brief The settings for the window.
 */
typedef struct ac_window_settings_t {
    /**
     * Window width.
     */
    int width;
    /**
     * Window height.
     */
    int height;
    /**
     * Window title.
     */
    const char* title;

    /**
     * Whether the window should be fullscreen.
     * If true, width and height are ignored.
     */
    bool fullscreen;
    /**
     * Whether the window should be resizable.
     * Does not apply to fullscreen windows.
     * If true, width and height are used in initial window creation.
     */
    bool resizable;
    /**
     * FPS limit for the window.
     * If 0, no limit is set.
     */
    uint32_t fps;
} ac_window_settings_t;

/**
 * Window object.
 */
typedef struct ac_window_t ac_window_t;

/**
 * Initializes the window.
 * @param settings The settings for the window.
 * @return The window object.
 */
ac_window_t* ac_window_init(ac_window_settings_t* settings);

/**
 * Updates the window.
 * @param window The window object.
 * @param update The user-defined update function.
 * @param user_data User-defined data to pass to the update function.
 */
void ac_window_update(ac_window_t* window, void (*update)(void* user_data), void* user_data);

/**
 * Shuts down the window.
 * @param window The window object.
 * @param shutdown The user-defined shutdown function.
 * @param user_data User-defined data to pass to the shutdown function.
 */
void ac_window_shutdown(ac_window_t* window, void (*shutdown)(void* user_data), void* user_data);

/**
 * Gets if the window is running.
 * @param window The window object.
 * @return True if the window is running, false otherwise.
 */
bool ac_window_is_running(ac_window_t* window);

/**
 * Checks if the window is fullscreen.
 * @param window The window object.
 * @return True if the window is fullscreen, false otherwise.
 */
bool ac_window_is_fullscreen(ac_window_t* window);

/**
 * Sets the window to fullscreen or windowed mode.
 * @param window The window object.
 * @param fullscreen True to set the window to fullscreen, false for windowed.
 */
void ac_window_set_fullscreen(ac_window_t* window, bool fullscreen);

/**
 * Gets the SDL window.
 * @param window The window object.
 * @return The SDL window.
 */
struct SDL_Window* ac_window_get_sdl_window(ac_window_t* window);

/**
 * Gets the Vulkan surface for the window.
 * Run this after initializing the window and before calling ac_window_update
 * for the first time.
 * @param window The window object.
 * @param instance The Vulkan instance.
 * @return The Vulkan surface.
 */
VkSurfaceKHR ac_window_get_surface(ac_window_t* window, VkInstance instance);

#endif  // AC_WINDOWING_H
