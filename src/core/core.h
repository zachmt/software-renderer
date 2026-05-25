#pragma once

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

#define Swap(type, x, y) do { type _temp_ = (x); (x) = (y); (y) = _temp_; } while(0)
#define Min(A,B) (((A)<(B))?(A):(B))
#define Max(A,B) (((A)>(B))?(A):(B))
#define AlignPow2(x,b) (((x) + (b) - 1)&(~((b) - 1)))
#define AlignDownPow2(x,b) ((x)&(~((b) - 1)))
#define AlignPadPow2(x,b)  ((0-(x)) & ((b) - 1))
#define IsPow2(x) ((x)!=0 && ((x)&((x)-1))==0)

#if COMPILER_MSVC
# define AlignOf(T) __alignof(T)
#elif COMPILER_CLANG
# define AlignOf(T) __alignof(T)
#elif COMPILER_GCC
# define AlignOf(T) __alignof__(T)
#endif

u64 round_down(u64 x, u64 multiple);
u64 round_up(u64 x, u64 multiple);
i32 round_float_to_int(f32 x);

typedef union{
    struct {
        f32 x;
        f32 y;
    };
    f32 v[2];
} Vec2;
f32 Vec2Distance(Vec2 v, Vec2 w);
bool32 Vec2IsEqual(Vec2 v, Vec2 w);
f32 Vec2DotProduct(Vec2 v, Vec2 w);
f32 Vec2Magnitude(Vec2 v);
f32 Vec2Direction(Vec2 v);

typedef union{
    struct {
        f32 x;
        f32 y;
        f32 z;
    };
    f32 v[3];
} Vec3;
f32 Vec3DotProduct(Vec3 v, Vec3 w);
Vec3 Vec3CrossProduct(Vec3 v, Vec3 w);
f32 Vec3Magnitude(Vec3 v);

typedef union{
    struct {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
    f32 v[4];
} Vec4;
f32 Vec4DotProduct(Vec4 v, Vec4 w);
f32 Vec4Magnitude(Vec4 v);

typedef union {
    struct {
        f32 m00, m01;
        f32 m10, m11;
    };
    f32 v[4];
    f32 m[2][2];
} Mat2;
Vec2 Mat2Vec2Multiply(Mat2 m, Vec2 v);
Mat2 Mat2Transpose(Mat2 m);
f32 Mat2Determinant(Mat2 m);

typedef union {
    struct {
        f32 m00, m01, m02;
        f32 m10, m11, m12;
        f32 m20, m21, m22;
    };
    f32 v[9];
    f32 m[3][3];
} Mat3;
Vec3 Mat3Vec3Multiply(Mat3 m, Vec3 v);
Mat3 Mat3Transpose(Mat3 m);

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
Vec4 Mat4Vec4Multiply(Mat4 m, Vec4 v);
Mat4 Mat4Transpose(Mat4 m);
