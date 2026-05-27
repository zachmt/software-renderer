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
    Arena *arena;
    ColorRGBA *frame_buffer;
    u64 frame_buffer_len;
    f32 *depth_buffer;
    u64 depth_buffer_len;
    u32 window_width;
    u32 window_height;
    Model *model;
} RenderState;

typedef struct {
    Vec3 position;
    Quat rotation;
    f32 fov_y_radians;
    f32 near_clip;
    f32 far_clip;
} Camera;

typedef struct {
    Model *model;
    f32 scale;
    Vec3 position;
    Quat rotation;
} Object;

static void update_and_render(RenderState *state);
