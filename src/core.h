#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef uint32_t bool32;

typedef int32_t ExitStatus;

#if defined(__clang__)
#define COMPILER_CLANG 1
#elif defined(_MSC_VER)
#define COMPILER_MSVC 1
#elif defined(__GNUC__) || defined(__GNUG__)
#define COMPILER_GCC 1
#endif

#define swap(type, x, y) do { type _temp_ = (x); (x) = (y); (y) = _temp_; } while(0)
#define min(A,B) (((A)<(B))?(A):(B))
#define max(A,B) (((A)>(B))?(A):(B))
#define align_pow2(x,b) (((x) + (b) - 1)&(~((b) - 1)))
#define align_down_pow2(x,b) ((x)&(~((b) - 1)))
#define align_pad_pow2(x,b)  ((0-(x)) & ((b) - 1))
#define is_pow2(x) ((x)!=0 && ((x)&((x)-1))==0)

#define runtime_assert(cond) do { assert((cond)); } while(0)

#if COMPILER_MSVC
# define align_of(T) __alignof(T)
#elif COMPILER_CLANG
# define align_of(T) __alignof(T)
#elif COMPILER_GCC
# define align_of(T) __alignof__(T)
#endif

static void rng_seed(u32 seed);
static i32 rng_generate_i32(void);
static f32 rng_generate_01(void);

typedef struct ScratchFrame ScratchFrame;
struct ScratchFrame{
    ScratchFrame *prev;
    void *frame_base;
};

typedef struct {
    void *memory_region_start;
    u64 bytes_allocated;
    u64 bytes_reserved;
    u64 bytes_committed;
    ScratchFrame *current_scratch_frame;
} Arena;

static Arena *get_scratch(Arena *arena);
static void free_scratch(Arena *scratch);

static void *arena_push(Arena *arena, u64 size, u64 alignment);
static void arena_clear(Arena *arena);
static void arena_destroy(Arena *arena);
static void arena_pop(Arena *arena, u64 size);

#define arena_push_struct(arena, typename) arena_push((arena), sizeof(typename), align_of(typename))
#define arena_push_array(arena, typename, count) arena_push((arena), sizeof(typename) * (count), align_of(typename))

typedef struct {
    u8 *data;
    u64 len;
} Str8;

typedef struct Str8Node Str8Node;
struct Str8Node {
    Str8Node *prev;
    Str8Node *next;
    Str8 str;
};

#define ws_delims s(" \n\r\t\v\f")
#define ws_except_nl_delims s(" \t\v\f")

static Str8 s(char *cstr);
static Str8 str8_cat(Arena *arena, Str8 a, Str8 b);
static Str8 str8_copy(Arena *arena, Str8 s);
static Str8 str8_trim(Str8 s);
static Str8Node *str8_split(Arena *arena, Str8 s, Str8 delims);
static bool32 is_whitespace(u8 c);
static bool32 str8_contains(Str8 s, u8 c);
static bool32 str8_ends_with(Str8 s, Str8 suffix);
static bool32 str8_equal(Str8 a, Str8 b);
static bool32 str8_starts_with(Str8 s, Str8 prefix);
static char *cstr(Arena *arena, Str8 s);
static f32 str8_parse_f32(Str8 s);
static i32 str8_parse_i32(Str8 s);
static void println(Str8 s);
