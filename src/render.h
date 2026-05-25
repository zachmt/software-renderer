#pragma once

#include "core/core.h"
#include "core/arena.h"
#include "model.h"
#include <stddef.h>

typedef union {
    struct {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    };
    u8 vals[4];
} ColorRGBA;

typedef struct {
    Arena *arena;
    ColorRGBA *frame_buffer;
    size_t frame_buffer_len;
    u32 window_width;
    u32 window_height;
    Model *model;
} RenderState;

void update_and_render(RenderState *state);
