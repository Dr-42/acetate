#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_vulkan.h>

#include "core/ac_mem.h"
#include "core/ac_log.h"
#include "interface/ac_windowing.h"

typedef struct ac_window_t {
    ac_window_settings_t settings;
    SDL_Window* window;
    bool initialized;
    bool running;
    bool is_minimized;
} ac_window_t;

ac_window_t* ac_window_init(ac_window_settings_t* settings) {
    ac_window_t* window = ac_malloc(sizeof(ac_window_t), AC_MEM_ENTRY_INTERFACE);
    window->settings = *settings;
    SDL_WindowFlags flags = SDL_WINDOW_VULKAN;
    if (settings->fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
    if (settings->resizable) flags |= SDL_WINDOW_RESIZABLE;
    SDL_Init(SDL_INIT_VIDEO);
    window->window =
        SDL_CreateWindow(settings->title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, settings->width, settings->height, flags);
    if (!window->window) {
        ac_free(window);
        ac_log_fatal_exit("Failed to create window");
        return NULL;
    }
    window->initialized = true;
    return window;
}

void ac_window_draw() {
    // This function is intentionally left blank
}

void ac_window_update(ac_window_t* window, void (*update)(void* user_data), void* user_data) {
    static uint32_t last_time = 0;
    uint32_t current_time = SDL_GetTicks();
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            window->running = false;
            return;
        }
        if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                window->is_minimized = true;
            }
            if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
                window->is_minimized = false;
            }
        }
    }
    if (!window->is_minimized) {
        ac_window_draw();
        if (update) update(user_data);
        if (window->settings.fps > 0) {
            uint32_t frame_time = 1000 / window->settings.fps;
            if (current_time - last_time < frame_time) {
                SDL_Delay(frame_time - (current_time - last_time));
            }
        }
    } else {
        SDL_Delay(100);
    }
}
void ac_window_shutdown(ac_window_t* window, void (*shutdown)(void* user_data), void* user_data) {
    if (!window->initialized) return;
    SDL_DestroyWindow(window->window);
    SDL_Quit();
    ac_free(window);
    if (shutdown) shutdown(user_data);
}

bool ac_window_is_running(ac_window_t* window) { return window->running; }

bool ac_window_is_fullscreen(ac_window_t* window) { return (SDL_GetWindowFlags(window->window) & SDL_WINDOW_FULLSCREEN) != 0; }

void ac_window_set_fullscreen(ac_window_t* window, bool fullscreen) {
    SDL_SetWindowFullscreen(window->window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
}

SDL_Window* ac_window_get_sdl_window(ac_window_t* window) { return window->window; }

VkSurfaceKHR ac_window_get_surface(ac_window_t* window, VkInstance instance) {
    VkSurfaceKHR surface;
    SDL_Vulkan_CreateSurface(window->window, instance, &surface);
    return surface;
}
