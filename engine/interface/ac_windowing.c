#include "interface/ac_windowing.h"

#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_vulkan.h>

#include "core/ac_mem.h"
#include "core/ac_log.h"
#include "vk_man/ac_vulkan.h"

typedef struct ac_window_t {
    ac_window_settings_t settings;
    SDL_Window* window;
    bool running;
    bool is_minimized;
    ac_vk_data* vk_data;
} ac_window_t;

ac_window_t* ac_window_init(ac_window_settings_t* settings) {
    // Check if host is running wayland. If so, use wayland windowing system
    char* wayland_display = getenv("XDG_SESSION_TYPE");
    ac_log_debug("XDG_SESSION_TYPE: %s", wayland_display);
    if (wayland_display && strcmp(wayland_display, "wayland") == 0) {
        SDL_setenv("SDL_VIDEODRIVER", "wayland", 1);
        ac_log_debug("Wayland windowing system enabled");
    }
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
    window->vk_data = ac_vk_init(settings->title, true, ac_window_get_sdl_window(window));
    window->running = true;
    return window;
}

void ac_window_draw(ac_vk_data* vk_data) {
    ac_vk_draw_frame(vk_data);
    // getchar();
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
        if (e.type == SDL_KEYUP) {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                window->running = false;
                return;
            }
        }
    }
    if (!window->is_minimized) {
        ac_window_draw(window->vk_data);
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
    ac_vk_cleanup(window->vk_data);
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
