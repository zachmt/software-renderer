#pragma once

#include "core/core.h"
#include "core/arena.h"
#include "model.h"

typedef struct {
    Arena *arena;
    Mesh *mesh;
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

void UpdateAndRender(u8 *frame_buffer, u64 frame_buffer_len, u32 width, u32 height, GameState *state);
