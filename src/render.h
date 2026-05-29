#pragma once

#include "core.h"
#include "arena.h"
#include "model.h"

typedef union {
    struct {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    };
    u8 vals[4];
} ColorRGBA;

typedef union {
    struct {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };
    f32 vals[4];
} ColorRGBAf;

typedef struct {
    Vec3 position;
    Quat rotation;
    f32 fov_y_radians;
    f32 near_clip;
    f32 far_clip;
    f32 aspect_ratio;
} Camera;

typedef struct {
    Model *model;
    f32 scale;
    Vec3 position;
    Quat rotation;
} Object;

typedef struct {
    f32 forward_backward;
    f32 left_right;
    f32 up_down;
    f32 look_horizontal;
    f32 look_vertical;
} Controls;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
    u8 color[4];
} EasyFontVertex;

typedef struct {
    Arena *arena;
    ColorRGBA *frame_buffer;
    u64 frame_buffer_len;
    f32 *depth_buffer;
    u64 depth_buffer_len;
    u32 window_width;
    u32 window_height;
    Object *obj;
    Camera camera;
    Controls controls;
} RenderState;

static void update_and_render(RenderState *state);
