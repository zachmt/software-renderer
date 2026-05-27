#define RGFW_IMPLEMENTATION
#include "RGFW.h"

#include "core.h"
#include "arena.h"
#include "model.h"
#include "render.h"

int main(void) {
    const i32 window_width = 800;
    const i32 window_height = 450;
    RGFW_window *window = RGFW_createWindow("Software Renderer", 0, 0, window_width, window_height, RGFW_windowCenter | RGFW_windowTransparent | RGFW_windowNoResize);
    RGFW_window_setExitKey(window, RGFW_keyEscape);


    RenderState state = {0};
    state.arena = arena_init();
    state.window_width = window_width;
    state.window_height = window_height;
    state.frame_buffer_len = state.window_width * state.window_height;
    state.frame_buffer = (ColorRGBA *)arena_push(state.arena, state.frame_buffer_len * sizeof(ColorRGBA), AlignOf(ColorRGBA));
    state.model = mesh_from_obj(state.arena, "res/model.obj");

    RGFW_surface *surface = RGFW_createSurface((u8 *)state.frame_buffer, (i32)state.window_width, (i32)state.window_height, RGFW_formatRGBA8);

    while (RGFW_window_shouldClose(window) == RGFW_FALSE) {
        RGFW_pollEvents();

        i32 w, h;
        RGFW_window_getSize(window, (i32 *)&w, (i32 *)&h);

        update_and_render(&state);

        RGFW_window_blitSurface(window, surface);
    }

    arena_destroy(state.arena);

    RGFW_surface_free(surface);
    RGFW_window_close(window);
}

#include "os/linux/wayland/wayland_source_files.h"
#include "os/linux/os_core.c"
#include "render.c"
#include "model.c"
#include "core.c"
#include "arena.c"
#include "maths.c"
