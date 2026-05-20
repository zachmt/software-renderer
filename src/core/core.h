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

typedef union{
    struct {
        f32 x;
        f32 y;
    };
    f32 v[2];
} Vec2;


typedef union{
    struct {
        f32 x;
        f32 y;
        f32 z;
    };
    f32 v[3];
} Vec3;

typedef union{
    struct {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
    f32 v[4];
} Vec4;

typedef union {
    struct {
        f32 m00, m01;
        f32 m10, m11;
    };
    f32 v[4];
    f32 m[2][2];
} Mat2;

typedef union {
    struct {
        f32 m00, m01, m02;
        f32 m10, m11, m12;
        f32 m20, m21, m22;
    };
    f32 v[9];
    f32 m[3][3];
} Mat3;

typedef union {
    struct {
        f32 m00, m01, m02, m03;
        f32 m10, m11, m12, m13;
        f32 m20, m21, m22, m23;
        f32 m30, m31, m32, m33;
    };
    f32 v[16];
    f32 m[4][4];
} Mat4;
