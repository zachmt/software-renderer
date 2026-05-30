#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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
