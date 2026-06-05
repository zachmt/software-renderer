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

#define ARENA_MAX_RESERVATION (1024ULL * 1024ULL * 1024ULL * 4ULL)
static i32 active_arenas = 0;

static Arena *get_scratch(Arena *arena) {
    static Arena scratch_arena_global[2] = {0};

    // Use a scratch arena that won't conflict
    Arena *scratch = (arena == &scratch_arena_global[0]) ? &scratch_arena_global[1] : &scratch_arena_global[0];
    // Initialize the arena if not yet initialized
    arena_push(scratch, 0, align_of(u8));

    // Find the new frame base
    void *base = (u8*)scratch->memory_region_start  + scratch->bytes_allocated;

    // Create new scratch frame
    ScratchFrame *new_scratch_frame = arena_push_struct(scratch, ScratchFrame);
    new_scratch_frame->prev = scratch->current_scratch_frame;
    new_scratch_frame->frame_base = base;
    scratch->current_scratch_frame = new_scratch_frame;

    return scratch;
}

static void free_scratch(Arena *scratch) {
    // By definition the current scratch frame can't be zero because free_scratch must be called after get_scratch
    runtime_assert(scratch->current_scratch_frame != 0);

    u64 size = (u64)(((u8*)scratch->memory_region_start + scratch->bytes_allocated) - (u8*)scratch->current_scratch_frame->frame_base);
    scratch->current_scratch_frame = scratch->current_scratch_frame->prev;
    arena_pop(scratch, size);
}

static void *arena_push(Arena *arena, u64 size, u64 alignment) {
    runtime_assert(arena);
    runtime_assert(is_pow2(alignment));

    if (arena->memory_region_start == 0) {
        const u64 reservation_size = ARENA_MAX_RESERVATION;
        arena->memory_region_start = os_mem_reserve(reservation_size);
        arena->bytes_allocated = 0;
        arena->bytes_reserved = reservation_size;
        arena->bytes_committed = 0;
        arena->current_scratch_frame = 0;
        active_arenas++;
    }

    u8 *unallocated_start = (u8 *)arena->memory_region_start + arena->bytes_allocated;
    size += align_pad_pow2((u64)unallocated_start, alignment);

    runtime_assert(arena->bytes_allocated + size < arena->bytes_reserved);

    if (arena->bytes_allocated + size > arena->bytes_committed) {
        u8 *uncommitted_start = (u8 *)arena->memory_region_start + arena->bytes_committed;
        u8 *page_start = (u8 *)align_down_pow2((u64)uncommitted_start, os_get_pagesize());
        u64 diff = (u64)(uncommitted_start - page_start);
        u64 commit_len = align_pow2(size + diff, os_get_pagesize());
        os_mem_commit(page_start, commit_len);
        arena->bytes_committed += commit_len;
    }

    void *res = (u8 *)align_pow2((u64)arena->memory_region_start + arena->bytes_allocated, alignment);
    arena->bytes_allocated += size;
    memset(res, 0, size);


    // printf("Arena allocated %lu bytes.\tTotal allocated: %lu bytes.\tTotal committed: %lu bytes.\tTotal reserved: %lu bytes.\n", size, arena->bytes_allocated, arena->bytes_committed, arena->bytes_reserved);
    return res;
}

static void arena_clear(Arena *arena) {
    runtime_assert(arena);
    arena->bytes_allocated = 0;
}

static void arena_destroy(Arena *arena) {
    runtime_assert(arena);
    if (arena->memory_region_start != 0) {
        os_mem_free(arena->memory_region_start, arena->bytes_reserved);
    }
    active_arenas--;
    // printf("Arena destroyed. Active arenas: %i\n", active_arenas);
}

static void arena_pop(Arena *arena, u64 size) {
    runtime_assert(arena);
    runtime_assert(size <= arena->bytes_allocated);
    arena->bytes_allocated -= size;
}

static Str8 s(char *cstr) {
    runtime_assert(cstr != NULL);
    return (Str8){
        .data = (u8 *)cstr,
        .len = strlen(cstr),
    };
}

static Str8 str8_cat(Arena *arena, Str8 a, Str8 b) {
    runtime_assert(a.data != 0 || a.len == 0);
    runtime_assert(b.data != 0 || b.len == 0);
    // TODO: handle empty strings situations
    Str8 res = {0};
    res.len = a.len + b.len;
    res.data = arena_push(arena, res.len, align_of(u8));
    for (u32 i = 0; i < a.len; i++) {
        res.data[i] = a.data[i];
    }
    for (u64 i = a.len; i < res.len; i++) {
        res.data[i] = b.data[i-a.len];
    }
    return res;
}

static Str8 str8_copy(Arena *arena, Str8 s) {
    runtime_assert(s.data != 0 || s.len == 0);
    Str8 res = (Str8){0};
    if (s.len > 0) {
        res.data = arena_push_array(arena, u8, s.len);
        memcpy(res.data, s.data, s.len);
        res.len = s.len;
    }
    return res;
}

static Str8 str8_trim(Str8 s) {
    runtime_assert(s.data != 0 || s.len == 0);
    while (s.len > 0 && is_whitespace(*s.data)) {
        s.data++;
        s.len--;
    }
    while (s.len > 0 && is_whitespace(s.data[s.len-1])) {
        s.len--;
    }
    return s;
}

static Str8Node *str8_split(Arena *arena, Str8 s, Str8 delims) {
    runtime_assert(s.data != 0 || s.len == 0);
    runtime_assert(delims.data != 0 || delims.len == 0);
    Str8Node *head = arena_push(arena, sizeof(Str8Node), align_of(Str8Node));
    head->prev = 0;
    head->next = 0;

    u32 start = 0;
    while (start < s.len && str8_contains(delims, s.data[start])) {
        start++;
    }
    if (start == s.len) {
        head->str = s;
    } else {
        head->str.data = &(s.data[start]);
        head->str.len = 1;
        Str8Node *current = head;

        for (u32 i = start+1; i < s.len; i++) {
            if (!str8_contains(delims, s.data[i])) {
                current->str.len++;
            } else {
                while (i < s.len && str8_contains(delims, s.data[i])) {
                    i++;
                }
                if (i < s.len) {
                    current->next = arena_push(arena, sizeof(Str8Node), align_of(Str8Node));
                    current->next->prev = current;
                    current->next->next = 0;
                    current= current->next;

                    current->str.data = &(s.data[i]);
                    current->str.len = 1;
                }
            }
        }
    }
    return head;
}

static bool32 is_whitespace(u8 c) {
    return (c == ' ') || (c == '\n') || (c == '\r') || (c == '\t') || (c  == '\v') || (c == '\f');
}

static bool32 str8_contains(Str8 s, u8 c) {
    runtime_assert(s.data != 0 || s.len == 0);
    for (u32 i = 0; i < s.len; i++) {
        if (s.data[i] == c) {
            return true;
        }
    }
    return false;
}

static bool32 str8_ends_with(Str8 s, Str8 suffix) {
    runtime_assert(s.data != 0 || s.len == 0);
    if (s.len < suffix.len) {
        return false;
    }
    for (u32 i = 0; i < suffix.len; i++) {
        if (s.data[s.len-1-i] != suffix.data[suffix.len-1-i]) {
            return false;
        }
    }
    return true;
}

static bool32 str8_equal(Str8 a, Str8 b) {
    runtime_assert(a.data != 0 || a.len == 0);
    runtime_assert(b.data != 0 || b.len == 0);
    if (a.len != b.len) {
        return false;
    }
    for (u32 i = 0; i < a.len; i++) {
        if (a.data[i] != b.data[i]) {
            return false;
        }
    }
    return true;
}

static bool32 str8_starts_with(Str8 s, Str8 prefix) {
    runtime_assert(s.data != 0 || s.len == 0);
    if (s.len < prefix.len) {
        return false;
    }
    for (u32 i = 0; i < prefix.len; i++) {
        if (s.data[i] != prefix.data[i]) {
            return false;
        }
    }
    return true;
}

static char *cstr(Arena *arena, Str8 s) {
    runtime_assert(s.data != 0 || s.len == 0);
    char *res = arena_push(arena, s.len + 1, align_of(char));
    for (u32 i = 0; i < s.len; i++) {
        res[i] = (char)s.data[i];
    }
    res[s.len] = '\0';
    return res;
}

static f32 str8_parse_f32(Str8 s) {
    runtime_assert(s.data != 0 || s.len == 0);
    f32 res = 0.0f; 
    if (s.len > 0) {
        Arena *scratch = get_scratch(0);
        res = (f32)atof(cstr(scratch, s));
        free_scratch(scratch);
    }
    return res;
}

static i32 str8_parse_i32(Str8 s) {
    runtime_assert(s.data != 0);
    i32 res = 0;
    if (s.len > 0) {
        Arena *scratch = get_scratch(0);
        res = atoi(cstr(scratch, s));
        free_scratch(scratch);
    }
    return res;
}

static void println(Str8 s) {
    runtime_assert(s.data != 0 || s.len == 0);
    printf("%.*s\n", (i32)s.len, s.data);
}
