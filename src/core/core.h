#pragma once

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

#define SWAP(type, x, y) do { type _temp_ = (x); (x) = (y); (y) = _temp_; } while(0)

typedef struct {
    f32 x;
    f32 y;
} Vec2;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} Vec3;

typedef union{
    struct {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
    f32 vals[4];
} Vec4;
