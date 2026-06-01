#include "core.h"
#include "os.h"
#include <stdlib.h>

static void rng_seed(u32 seed) {
    srand(seed);
}

static i32 rng_generate_i32(void) {
    return rand();
}

static f32 rng_generate_01(void) {
    return ((f32)(rand()) / (f32)RAND_MAX);
}

static Arena *arena_init(void) {
    const u64 reservation_size = 1024ULL * 1024ULL * 1024ULL * 64ULL;
    void *base = os_mem_reserve(reservation_size);
    os_mem_commit(base, sizeof(Arena));
    Arena *arena = (Arena *)base;
    arena->memory_region_start = (u8 *)(arena + 1);
    arena->bytes_allocated = sizeof(Arena);
    arena->bytes_reserved = reservation_size;
    arena->bytes_committed = sizeof(Arena);
    return arena;
}

static void *arena_push(Arena *arena, u64 size, u64 alignment) {
    runtime_assert(is_pow2(alignment));
    u8 *unallocated_start = (u8 *)arena->memory_region_start + arena->bytes_allocated;
    size += align_pad_pow2((u64)unallocated_start, alignment);
    runtime_assert(arena->bytes_allocated + size < arena->bytes_reserved);
    if (arena->bytes_allocated + size > arena->bytes_committed) {
        u8 *uncommitted_start = (u8 *)arena->memory_region_start + arena->bytes_committed;
        u8 *page_start = (u8 *)align_down_pow2((u64)uncommitted_start, os_get_pagesize());
        u64 diff = (u64)(uncommitted_start - page_start);
        os_mem_commit(page_start, size + diff);
        arena->bytes_committed += size + diff;
    }
    void *res = (u8 *)align_pow2((u64)arena->memory_region_start + arena->bytes_allocated, alignment);
    arena->bytes_allocated += size;
    return res;
}

static void arena_clear(Arena *arena) {
    arena->bytes_allocated = sizeof(Arena);
}

static void arena_destroy(Arena *arena) {
    os_mem_free(arena, arena->bytes_reserved);
}
