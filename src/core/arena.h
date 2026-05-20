#pragma once

#include "core/core.h"

typedef struct {
    u8 *buffer;
    u64 size;
    u64 capacity;
    u64 committed;
} Arena;

Arena *arena_init(void);
u8 *arena_push(Arena *arena, u32 size);
void arena_shrink(Arena *arena, u32 size);
void arena_clear(Arena *arena);
void arena_destroy(Arena *arena);
