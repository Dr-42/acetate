#include <core/ac_log.h>
#include <core/ac_mem.h>
#include <core/ac_trace.h>
#include <interface/ac_windowing.h>

#include <signal.h>
#include <stdlib.h>
#include <vk_man/ac_vulkan.h>

void segfaulter(int sig) {
    ac_log_fatal("Segmentation fault caught : %d\n", sig);
    ac_print_trace(2);
    exit(1);
}

int main() {
    signal(SIGSEGV, segfaulter);
    ac_window_settings_t settings = {
        .width = 800,
        .height = 600,
        .title = "Hello, World!",
        .fullscreen = false,
        .resizable = true,
        .fps = 60,
    };

    ac_window_t* window = ac_window_init(&settings);
    if (!window) {
        ac_log_fatal_exit("Failed to initialize window\n");
    }

    ac_vk_data* vk_data = ac_vk_init(settings.title, true, ac_window_get_sdl_window(window));

    while (ac_window_is_running(window)) {
        ac_window_update(window, NULL, NULL);
    }

    ac_vk_cleanup(vk_data);
    ac_window_shutdown(window, NULL, NULL);

    ac_mem_exit();
    return 0;
}
