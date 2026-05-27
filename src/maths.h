#pragma once
#include "core.h"

#define PI32 3.1459265f

static u64 round_down(u64 x, u64 multiple);
static u64 round_up(u64 x, u64 multiple);
static i32 round_f32_to_i32(f32 x);

static i32 i32_abs(i32 x);
static f32 f32_abs(f32 x);

typedef union{
    struct {
        f32 x;
        f32 y;
    };
    f32 v[2];
} Vec2;
static Vec2 Vec2Normalize(Vec2 v);
static Vec2 Vec2Scale(Vec2 v, f32 n);
static bool32 Vec2IsEqual(Vec2 v, Vec2 w);
static f32 Vec2Direction(Vec2 v);
static f32 Vec2Distance(Vec2 v, Vec2 w);
static f32 Vec2DotProduct(Vec2 v, Vec2 w);
static f32 Vec2Magnitude(Vec2 v);

typedef union{
    struct {
        f32 x;
        f32 y;
        f32 z;
    };
    f32 v[3];
} Vec3;
static Vec3 Vec3CrossProduct(Vec3 v, Vec3 w);
static Vec3 Vec3Normalize(Vec3 v);
static Vec3 Vec3Scale(Vec3 v, f32 n);
static bool32 Vec3IsEqual(Vec3 v, Vec3 w);
static f32 Vec3DotProduct(Vec3 v, Vec3 w);
static f32 Vec3Magnitude(Vec3 v);

typedef union{
    struct {
        f32 x;
        f32 y;
        f32 z;
        f32 w;
    };
    f32 v[4];
} Vec4;
static Vec4 Vec4Normalize(Vec4 v);
static Vec4 Vec4Scale(Vec4 v, f32 n);
static bool32 Vec4IsEqual(Vec4 v, Vec4 w);
static f32 Vec4DotProduct(Vec4 v, Vec4 w);
static f32 Vec4Magnitude(Vec4 v);

typedef union {
    struct {
        f32 m00, m01;
        f32 m10, m11;
    };
    f32 v[4];
    f32 m[2][2];
} Mat2;
static Mat2 Mat2Identity(void);
static Mat2 Mat2Multiply(Mat2 m, Mat2 n);
static Mat2 Mat2Transpose(Mat2 m);
static Vec2 Mat2Vec2Multiply(Mat2 m, Vec2 v);
static f32 Mat2Determinant(Mat2 m);

typedef union {
    struct {
        f32 m00, m01, m02;
        f32 m10, m11, m12;
        f32 m20, m21, m22;
    };
    f32 v[9];
    f32 m[3][3];
} Mat3;
static Mat3 Mat3Identity(void);
static Mat3 Mat3Multiply(Mat3 m, Mat3 n);
static Mat3 Mat3Transpose(Mat3 m);
static Vec3 Mat3Vec3Multiply(Mat3 m, Vec3 v);

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
static Mat4 Mat4Identity(void);
static Mat4 Mat4Multiply(Mat4 m, Mat4 n);
static Mat4 Mat4Transpose(Mat4 m);
static Vec4 Mat4Vec4Multiply(Mat4 m, Vec4 v);

typedef union{
    struct {
        f32 a;
        f32 b;
        f32 c;
        f32 d;
    };
    f32 q[4];
} Quat;
static Mat4 QuatToMat4(Quat q);
static Quat QuatConjugate(Quat q);
static Quat QuatIdentity(void);
