#pragma once

#include "core.h"

typedef struct {
    void *memory_region_start;
    u64 bytes_allocated;
    u64 bytes_reserved;
    u64 bytes_committed;
} Arena;

static Arena *arena_init(void);
static void *arena_push(Arena *arena, u64 size, u64 alignment);
static void arena_clear(Arena *arena);
static void arena_destroy(Arena *arena);
