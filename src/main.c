#define _DEFAULT_SOURCE // fixes madvise not being defined due to RGFW
#define RGFW_IMPLEMENTATION
#include "RGFW.h"

#include "assets.h"
#include "core.h"
#include "maths.h"
#include "render.h"

RenderState *init_state(Arena *arena, u32 window_width, u32 window_height);
RenderState *init_state(Arena *arena, u32 window_width, u32 window_height) {
  RenderState *state =
      arena_push(arena, sizeof(RenderState), align_of(RenderState));
  state->arena = arena;
  state->window_width = window_width;
  state->window_height = window_height;
  state->frame_buffer_len = state->window_width * state->window_height;
  state->frame_buffer =
      arena_push(state->arena, state->frame_buffer_len * sizeof(ColorRGBA),
                 align_of(ColorRGBA));
  state->depth_buffer_len = state->window_width * state->window_height;
  state->depth_buffer = arena_push(
      state->arena, state->depth_buffer_len * sizeof(f32), align_of(f32));
  state->obj = arena_push(state->arena, sizeof(Object), align_of(Object));
  state->obj->scale = 1.0f;
  state->obj->position = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f};
  state->obj->rotation = quat_identity;
  state->obj->model = model_from_obj(state->arena, s("res/model.obj"));
  state->camera.position = (Vec4){.x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f};
  state->camera.rotation = quat_identity;
  state->camera.fov_y_radians = (40.0f) * (f32_pi / 180.0f);
  state->camera.near_clip = 0.5f;
  state->camera.far_clip = 1000.0f;
  state->camera.aspect_ratio =
      (f32)state->window_width / (f32)state->window_height;
  return state;
}

int main(void) {
  i32 window_width = 800;
  i32 window_height = 450;
  RGFW_window *window = RGFW_createWindow(
      "Software Renderer", 0, 0, window_width, window_height,
      RGFW_windowCenter | RGFW_windowTransparent | RGFW_windowNoResize);
  RGFW_window_setExitKey(window, RGFW_keyEscape);

  RGFW_monitor *monitor = RGFW_window_getMonitor(window);
  runtime_assert(monitor != NULL);
  window_width = window_width * (i32)monitor->pixelRatio;
  window_height = window_height * (i32)monitor->pixelRatio;
  runtime_assert(window_width > 0);
  runtime_assert(window_height > 0);

  Arena state_arena = {0};
  RenderState *state =
      init_state(&state_arena, (u32)window_width, (u32)window_height);

  RGFW_surface *surface =
      RGFW_createSurface((u8 *)state->frame_buffer, (i32)state->window_width,
                         (i32)state->window_height, RGFW_formatRGBA8);
  runtime_assert(surface != NULL);
  RGFW_surface *depth_surface =
      RGFW_createSurface((u8 *)state->depth_buffer, (i32)state->window_width,
                         (i32)state->window_height, RGFW_formatRGBA8);
  runtime_assert(depth_surface != NULL);

  f32 dt = 0.0f;
  struct timespec start, end;
  bool32 first_frame = true;

  while (RGFW_window_shouldClose(window) == RGFW_FALSE) {
    RGFW_pollEvents();

    if (RGFW_isKeyDown(RGFW_keyW)) {
      state->controls.forward_backward = 1.0f;
    } else if (RGFW_isKeyDown(RGFW_keyS)) {
      state->controls.forward_backward = -1.0f;
    } else {
      state->controls.forward_backward = 0.0f;
    }

    if (RGFW_isKeyDown(RGFW_keyA)) {
      state->controls.left_right = -1.0f;
    } else if (RGFW_isKeyDown(RGFW_keyD)) {
      state->controls.left_right = 1.0f;
    } else {
      state->controls.left_right = 0.0f;
    }

    if (RGFW_isKeyDown(RGFW_keyR)) {
      state->controls.up_down = 1.0f;
    } else if (RGFW_isKeyDown(RGFW_keyF)) {
      state->controls.up_down = -1.0f;
    } else {
      state->controls.up_down = 0.0f;
    }

    if (RGFW_isKeyDown(RGFW_keyI)) {
      state->controls.look_pitch = 1.0f;
    } else if (RGFW_isKeyDown(RGFW_keyK)) {
      state->controls.look_pitch = -1.0f;
    } else {
      state->controls.look_pitch = 0.0f;
    }

    if (RGFW_isKeyDown(RGFW_keyJ)) {
      state->controls.look_yaw = 1.0f;
    } else if (RGFW_isKeyDown(RGFW_keyL)) {
      state->controls.look_yaw = -1.0f;
    } else {
      state->controls.look_yaw = 0.0f;
    }

    if (RGFW_isKeyDown(RGFW_keyP)) {
      state->controls.look_roll = -1.0f;
    } else if (RGFW_isKeyDown(RGFW_keySemicolon)) {
      state->controls.look_roll = 1.0f;
    } else {
      state->controls.look_roll = 0.0f;
    }

    if (RGFW_isKeyPressed(RGFW_keyB)) {
      arena_clear(state->arena);
      state = init_state(&state_arena, (u32)window_width, (u32)window_height);
    }

    i32 w, h;
    RGFW_window_getSize(window, (i32 *)&w, (i32 *)&h);

    clock_gettime(CLOCK_MONOTONIC, &end);
    if (!first_frame) {
      i64 s = end.tv_sec - start.tv_sec;
      i64 ns = end.tv_nsec - start.tv_nsec;
      dt = (f32)s + 0.000000001f * (f32)ns;
    }
    first_frame = false;
    clock_gettime(CLOCK_MONOTONIC, &start);
    update_and_render(state, dt);
    RGFW_window_blitSurface(window, surface);
  }

  arena_destroy(&state_arena);

  RGFW_surface_free(surface);
  RGFW_surface_free(depth_surface);
  RGFW_window_close(window);
}

#if OS_MAC
#include "os_mac.c"
#endif

#if OS_LINUX
#include "gen/wayland/wayland_source_files.h"
#include "os_linux.c"
#endif

#include "assets.c"
#include "core.c"
#include "maths.c"
#include "render.c"
