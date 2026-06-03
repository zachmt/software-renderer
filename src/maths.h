#pragma once
#include "core.h"

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

typedef union{
    struct {
        f32 a;
        f32 b;
        f32 c;
        f32 d;
    };
    struct {
        f32 w;
        f32 x;
        f32 y;
        f32 z;
    };
    f32 q[4];
} Quat;

#define f32_pi 3.14159265f

static f32 f32_tan(f32 radians);

static f32 f32_inf(void);
static f32 f32_neg_inf(void);

static i32 f32_round_to_i32(f32 x);

static i32 i32_abs(i32 x);
static f32 f32_abs(f32 x);

static Vec2 vec2_from_vec3(Vec3 v);
static Vec2 vec2_normalize(Vec2 v);
static Vec2 vec2_scale(Vec2 v, f32 n);
static bool32 vec2_is_equal(Vec2 v, Vec2 w);
static f32 vec2_direction(Vec2 v);
static f32 vec2_distance(Vec2 v, Vec2 w);
static f32 vec2_dot_product(Vec2 v, Vec2 w);
static f32 vec2_magnitude(Vec2 v);

#define vec2_zero ((Vec2){.x=0.0f,.y=0.0f})
#define vec2_ihat ((Vec2){.x=1.0f,.y=0.0f})
#define vec2_jhat ((Vec2){.x=0.0f,.y=1.0f})

static Vec3 vec3_cross_product(Vec3 v, Vec3 w);
static Vec3 vec3_from_vec2(Vec2 v, f32 z);
static Vec3 vec3_from_vec4(Vec4 v);
static Vec3 vec3_normalize(Vec3 v);
static Vec3 vec3_scale(Vec3 v, f32 n);
static bool32 vec3_is_equal(Vec3 v, Vec3 w);
static f32 vec3_angle_between(Vec3 v, Vec3 w);
static f32 vec3_dot_product(Vec3 v, Vec3 w);
static f32 vec3_magnitude(Vec3 v);

#define vec3_zero ((Vec3){.x=0.0f,.y=0.0f,.z=0.0f})
#define vec3_ihat ((Vec3){.x=1.0f,.y=0.0f,.z=0.0f})
#define vec3_jhat ((Vec3){.x=0.0f,.y=1.0f,.z=0.0f})
#define vec3_khat ((Vec3){.x=0.0f,.y=0.0f,.z=1.0f})

static Vec4 vec4_add(Vec4 v, Vec4 w);
static Vec4 vec4_from_vec3(Vec3 v, f32 w);
static Vec4 vec4_normalize(Vec4 v);
static Vec4 vec4_scale(Vec4 v, f32 n);
static bool32 vec4_is_equal(Vec4 v, Vec4 w);
static f32 vec4_dot_product(Vec4 v, Vec4 w);
static f32 vec4_magnitude(Vec4 v);

#define vec4_zero ((Vec4){.x=0.0f,.y=0.0f,.z=0.0f, .w=0.0f})
#define vec4_ihat ((Vec4){.x=1.0f,.y=0.0f,.z=0.0f, .w=0.0f})
#define vec4_jhat ((Vec4){.x=0.0f,.y=1.0f,.z=0.0f, .w=0.0f})
#define vec4_khat ((Vec4){.x=0.0f,.y=0.0f,.z=1.0f, .w=0.0f})

static Mat2 mat2_multiply(Mat2 m, Mat2 n);
static Mat2 mat2_transpose(Mat2 m);
static Vec2 mat2_vec2_multiply(Mat2 m, Vec2 v);
static f32 mat2_determinant(Mat2 m);

#define mat2_zero ((Mat2){0})
#define mat2_identity ((Mat2){.m00=1.0f,.m11=1.0f})

static Mat3 mat3_multiply(Mat3 m, Mat3 n);
static Mat3 mat3_transpose(Mat3 m);
static Vec3 mat3_vec3_multiply(Mat3 m, Vec3 v);

#define mat3_zero ((Mat3){0})
#define mat3_identity ((Mat3){.m00=1.0f,.m11=1.0f,.m22=1.0f})

static Mat4 mat4_multiply(Mat4 m, Mat4 n);
static Mat4 mat4_transpose(Mat4 m);
static Vec4 mat4_vec4_multiply(Mat4 m, Vec4 v);

#define mat4_zero ((Mat4){0})
#define mat4_identity ((Mat4){.m00=1.0f,.m11=1.0f,.m22=1.0f,.m33=1.0f})

static Mat4 quat_to_rotation_mat4(Quat q);
static Quat quat_conjugate(Quat q);
static Quat quat_from_axis_angle(f32 angle, Vec3 axis);
static Quat quat_multiply(Quat q, Quat r);
static Quat quat_normalize(Quat q);
static f32 quat_magnitude(Quat q);

#define quat_zero ((Quat){0})
#define quat_identity ((Quat){.w=1.0f})

