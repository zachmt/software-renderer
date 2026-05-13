#pragma once

#include "core.h"

typedef struct {
    bool iscool;
} GameState;

typedef union {
    struct {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    };
    u8 vals[4];
} ColorRGBA;

void render(u8 *frame_buffer, u32 frame_buffer_len, i32 width, i32 height, GameState *state);
