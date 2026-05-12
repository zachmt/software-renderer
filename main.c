#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "render.h"

#include "render.c"
#include "wayland_source_files.h"

#define RGFW_IMPLEMENTATION
#include "RGFW.h"

int main(void) {
    RGFW_window *window = RGFW_createWindow("Software Renderer", 0, 0, 1920, 1080, RGFW_windowCenter | RGFW_windowTransparent);
    RGFW_window_setExitKey(window, RGFW_keyEscape);

    RGFW_monitor *monitor = RGFW_window_getMonitor(window);
    uint32_t width = 1920;
    uint32_t height = 1080;

    GameState state = { .iscool = false };

    if (monitor) {
        width = (uint32_t)((float)monitor->mode.w * monitor->pixelRatio);
        height = (uint32_t)((float)monitor->mode.h * monitor->pixelRatio);
    }

    const uint32_t frame_buffer_len = width*height*4;
    uint8_t *frame_buffer = (uint8_t *)malloc((uint32_t)(frame_buffer_len));
    RGFW_surface *surface = RGFW_createSurface(frame_buffer, (int32_t)width, (int32_t)height, RGFW_formatRGBA8);

    uint32_t counter = 0;
    while (RGFW_window_shouldClose(window) == RGFW_FALSE) {
        printf("loop ran %u\n", counter++);
        RGFW_pollEvents();
        monitor = RGFW_window_getMonitor(window);

        uint32_t w, h;
        RGFW_window_getSize(window, (int32_t *)&w, (int32_t *)&h);

        render(frame_buffer, frame_buffer_len, w, h, &state);

        RGFW_window_blitSurface(window, surface);
    }

    RGFW_surface_free(surface);
    free(frame_buffer);
    RGFW_window_close(window);
}
