#define RGFW_IMPLEMENTATION
#include "RGFW.h"

#include "arena.h"
#include "core.h"
#include "maths.h"
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
    state.frame_buffer = arena_push(state.arena, state.frame_buffer_len * sizeof(ColorRGBA), AlignOf(ColorRGBA));
    state.depth_buffer_len = state.window_width * state.window_height;
    state.depth_buffer = arena_push(state.arena, state.depth_buffer_len * sizeof(f32), AlignOf(f32));
    state.obj = &(Object){
        .scale = 1.0f,
        .position = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
        .rotation = QuatIdentity(),
        .model = mesh_from_obj(state.arena, "res/model.obj"),
    };
    state.camera = (Camera){
        .position = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f},
        // .position = (Vec3){.x = 0.0f, .y = -2.0f, .z = 2.0f},
        // .rotation = QuatFromAxisAngle((-45.0f) * (PI32 / 180.0f), (Vec3){.x=1.0f,.y=0.0f,.z=0.0f}),
        .rotation = QuatIdentity(),
        .fov_y_radians = (40.0f) * (PI32 / 180.0f),
        .near_clip = 0.5f,
        .far_clip = 1000.0f,
        .aspect_ratio = (f32)state.window_width / (f32)state.window_height,
    };

    RGFW_surface *surface = RGFW_createSurface((u8 *)state.frame_buffer, (i32)state.window_width, (i32)state.window_height, RGFW_formatRGBA8);

    while (RGFW_window_shouldClose(window) == RGFW_FALSE) {
        RGFW_pollEvents();
        if (RGFW_isKeyDown(RGFW_keyW)) {
            state.controls.forward_backward = 1.0f;
        } else if (RGFW_isKeyDown(RGFW_keyS)) {
            state.controls.forward_backward = -1.0f;
        } else {
            state.controls.forward_backward = 0.0f;
        }

        if (RGFW_isKeyDown(RGFW_keyA)) {
            state.controls.left_right = -1.0f;
        } else if (RGFW_isKeyDown(RGFW_keyD)) {
            state.controls.left_right = 1.0f;
        } else {
            state.controls.left_right = 0.0f;
        }

        if (RGFW_isKeyDown(RGFW_keyR)) {
            state.controls.up_down = 1.0f;
        } else if (RGFW_isKeyDown(RGFW_keyF)) {
            state.controls.up_down = -1.0f;
        } else {
            state.controls.up_down = 0.0f;
        }




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
