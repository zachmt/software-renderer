#include "render.h"
#include "core.h"

#define RGFW_IMPLEMENTATION
#include "RGFW.h"

#include "wayland_source_files.h"

#include "render.c"

int main(void) {
    const i32 window_width = 500;
    const i32 window_height = 500;
    RGFW_window *window = RGFW_createWindow("Software Renderer", 0, 0, window_width, window_height, RGFW_windowCenter | RGFW_windowTransparent | RGFW_windowNoResize);
    RGFW_window_setExitKey(window, RGFW_keyEscape);

    const i32 frame_buffer_len = window_width*window_height*4;
    u8 *frame_buffer = (u8 *)malloc(frame_buffer_len);
    RGFW_surface *surface = RGFW_createSurface(frame_buffer, (i32)window_width, (i32)window_height, RGFW_formatRGBA8);

    GameState state = { .iscool = false };

    while (RGFW_window_shouldClose(window) == RGFW_FALSE) {
        RGFW_pollEvents();

        i32 w, h;
        RGFW_window_getSize(window, (i32 *)&w, (i32 *)&h);

        render(frame_buffer, frame_buffer_len, w, h, &state);

        RGFW_window_blitSurface(window, surface);
    }

    RGFW_surface_free(surface);
    free(frame_buffer);
    RGFW_window_close(window);
}
