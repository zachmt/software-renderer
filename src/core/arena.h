#pragma once

#include "core/core.h"

typedef struct {
    u8 *buffer;
    u64 size;
    u64 capacity;
    u64 committed;
} Arena;

Arena *ArenaInit(void);
u8 *ArenaPush(Arena *arena, u32 size);
void ArenaShrink(Arena *arena, u32 size);
void ArenaClear(Arena *arena);
void ArenaDestroy(Arena *arena);
