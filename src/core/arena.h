#pragma once

#include "core/core.h"

typedef struct {
    void *memory_region_start;
    u64 bytes_allocated;
    u64 bytes_reserved;
    u64 bytes_committed;
} Arena;

Arena *arena_init(void);
void *arena_push(Arena *arena, u64 size, u64 alignment);
// void arena_shrink(Arena *arena, u64 size);
void arena_clear(Arena *arena);
void arena_destroy(Arena *arena);
