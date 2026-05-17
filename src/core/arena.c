#include "core/arena.h"
#include "os/os_core.h"
#include <assert.h>

static u64 round_down(u64 x, u64 multiple) {
    assert(multiple != 0);
    return x - (x % multiple);
}

static u64 round_up(u64 x, u64 multiple) {
    assert(multiple != 0);
    u64 remainder = x % multiple;
    return (remainder == 0) ? x : x + multiple - remainder;
}

Arena *ArenaInit(void) {
    const u64 reservation_size = 1024ULL * 1024ULL * 1024ULL * 64ULL;
    u8 *base = (u8 *)os_mem_reserve(reservation_size);
    os_mem_commit(base, sizeof(Arena));
    Arena *arena = (Arena *)base;
    base += sizeof(Arena);
    arena->buffer = base;
    arena->size = 0;
    arena->capacity = reservation_size - sizeof(Arena);
    arena->committed = sizeof(Arena);
    return arena;
}

u8 *ArenaPush(Arena *arena, u32 size) {
    assert(arena->size + size < arena->capacity);
    if (arena->size + size > arena->committed) {
        u8 *base = (u8 *)round_down((u64)(arena->buffer + arena->committed), os_get_pagesize());
        u32 diff = arena->buffer + arena->committed - base;
        os_mem_commit(base, size + diff);
        arena->committed += size + diff;
    }
    u8 *res = arena->buffer + arena->size;
    arena->size += size;
    return res;
}

void ArenaShrink(Arena *arena, u32 size) {
    if (size > arena->size) {
        arena->size = 0;
    } else {
        arena->size -= size;
    }
}

void ArenaClear(Arena *arena) {
    arena->size = 0;
}

void ArenaDestroy(Arena *arena) {
    os_mem_free(arena, arena->capacity);
}
