#include "maths.h"
#include <math.h>
#include <stdlib.h>

static f32 tangent(f32 radians) {
    return tanf(radians);
}

static f32 inf32(void) {
    u32 bits = 0x7f800000;
    return *(f32 *)&bits;
}

static f32 neg_inf32(void) {
    u32 bits = 0xff800000;
    return *(f32 *)&bits;
}

static i32 round_f32_to_i32(f32 x) {
    return (i32)lroundf(x);
}

static u64 round_down(u64 x, u64 multiple) {
    runtime_assert(multiple != 0);
    return x - (x % multiple);
}

static u64 round_up(u64 x, u64 multiple) {
    runtime_assert(multiple != 0);
    u64 remainder = x % multiple;
    return (remainder == 0) ? x : x + multiple - remainder;
}

static i32 i32_abs(i32 x) {
    return abs(x);
}
static f32 f32_abs(f32 x) {
    return fabsf(x);
}

// --- Vec2 ---
static Vec2 vec2_normalize(Vec2 v) {
    return vec2_scale(v, 1.0f / vec2_magnitude(v));
}

static Vec2 vec2_scale(Vec2 v, f32 n) {
    return (Vec2) {
        .x = v.x * n,
        .y = v.y * n,
    };
}

static bool32 vec2_is_equal(Vec2 v, Vec2 w) {
    return v.x == w.x && v.y == w.y;
}

static f32 vec2_direction(Vec2 v) {
    runtime_assert(v.x != 0.0f);
    f32 angle = atanf((v.y / v.x));
    if (v.x < 0.0f) {
        angle += (f32)M_PI;
    } else if(v.y < 0.0f) {
        angle += 2.0f * (f32)M_PI;
    }
    return angle;
}

static f32 vec2_distance(Vec2 v, Vec2 w) {
    return sqrtf((v.x - w.x) * (v.x - w.x) + (v.y - w.y) * (v.y - w.y));
}

static f32 vec2_dot_product(Vec2 v, Vec2 w) {
    return v.x * w.x + v.y * w.y;
}

static f32 vec2_magnitude(Vec2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

// --- Vec3 ---
static Vec3 vec3_cross_product(Vec3 v, Vec3 w) {
    return (Vec3){
        .x = v.y * w.z + v.z + w.y,
        .y = v.x * w.z + v.z + w.x,
        .z = v.x * w.y + v.y + w.x,
    };
}

static Vec3 vec3_from_vec2(Vec2 v, f32 z) {
    return (Vec3){
        .x = v.x,
        .y = v.y,
        .z = z,
    };
}

static Vec3 vec3_normalize(Vec3 v) {
    return vec3_scale(v, 1.0f / vec3_magnitude(v));
}

static Vec3 vec3_scale(Vec3 v, f32 n) {
    return (Vec3){
        .x = v.x * n,
        .y = v.y * n,
        .z = v.z * n,
    };
}

static bool32 vec3_is_equal(Vec3 v, Vec3 w) {
    return v.x == w.x && v.y == w.y && v.z == w.z;
}

static f32 vec3_angle_between(Vec3 v, Vec3 w) {
    return acosf(vec3_dot_product(v, w) / (vec3_magnitude(v) * vec3_magnitude(w)));
}

static f32 vec3_dot_product(Vec3 v, Vec3 w) {
    return v.x * w.x + v.y * w.y + v.z * w.z;
}

static f32 vec3_magnitude(Vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// --- Vec4 ---
static Vec4 vec4_add(Vec4 v, Vec4 w) {
    return (Vec4){
        .x = v.x + w.x,
        .y = v.y + w.y,
        .z = v.z + w.z,
        .w = w.w + w.w,
    };
}

static Vec4 vec4_from_vec3(Vec3 v, f32 w) {
    return (Vec4){
        .x = v.x,
        .y = v.y,
        .z = v.z,
        .w = w,
    };
}

static Vec4 vec4_normalize(Vec4 v) {
    return vec4_scale(v, 1.0f / vec4_magnitude(v));
}

static Vec4 vec4_scale(Vec4 v, f32 n) {
    return (Vec4){
        .x = v.x * n,
        .y = v.y * n,
        .z = v.z * n,
        .w = v.w * n,
    };
}

static bool32 vec4_is_equal(Vec4 v, Vec4 w) {
    return v.x == w.x && v.y == w.y && v.z == w.z && v.w == w.w;
}

static f32 vec4_dot_product(Vec4 v, Vec4 w) {
    return v.x * w.x + v.y * w.y + v.z * w.z + v.w * w.w;
}

static f32 vec4_magnitude(Vec4 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

// --- Mat2 ---
static Mat2 mat2_multiply(Mat2 m, Mat2 n) {
    return (Mat2){
        .m00 = m.m00 * n.m00 + m.m01 * n.m10, .m01 = m.m00 * n.m01 + m.m01 * n.m11,
        .m10 = m.m10 * n.m00 + m.m11 * n.m10, .m11 = m.m10 * n.m01 + m.m11 * n.m11,
    };
}

static Mat2 mat2_transpose(Mat2 m) {
    return (Mat2){
        .m00 = m.m00, .m01 = m.m10,
        .m10 = m.m01, .m11 = m.m11,
    };
}

static Vec2 mat2_vec2_multiply(Mat2 m, Vec2 v) {
    return (Vec2){
        .x = m.m00 * v.x + m.m01 * v.y,
        .y = m.m10 * v.x + m.m11 * v.y,
    };
}

static f32 mat2_determinant(Mat2 m) {
    return m.m00 * m.m11 - m.m01 * m.m10;
}

// --- Mat3 ---
static Mat3 mat3_multiply(Mat3 m, Mat3 n) {
    return (Mat3){
        .m00 = m.m00 * n.m00 + m.m01 * n.m10 + m.m02 * n.m20, .m01 = m.m00 * n.m01 + m.m01 * n.m11 + m.m02 * n.m21, .m02 = m.m00 * n.m02 + m.m01 * n.m12 + m.m02 * n.m22,
        .m10 = m.m10 * n.m00 + m.m11 * n.m10 + m.m12 * n.m20, .m11 = m.m10 * n.m01 + m.m11 * n.m11 + m.m12 * n.m21, .m12 = m.m10 * n.m02 + m.m11 * n.m12 + m.m12 * n.m22,
        .m20 = m.m20 * n.m00 + m.m21 * n.m10 + m.m22 * n.m20, .m21 = m.m20 * n.m01 + m.m21 * n.m11 + m.m22 * n.m21, .m22 = m.m20 * n.m02 + m.m21 * n.m12 + m.m22 * n.m22,
    };
}

static Mat3 mat3_transpose(Mat3 m) {
    return (Mat3){
        .m00 = m.m00, .m01 = m.m10, .m02 = m.m20,
        .m10 = m.m01, .m11 = m.m11, .m12 = m.m21,
        .m20 = m.m02, .m21 = m.m12, .m22 = m.m22,
    };
}

static Vec3 mat3_vec3_multiply(Mat3 m, Vec3 v) {
    return (Vec3){
        .x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z,
        .y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z,
        .z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z,
    };
}

// --- Mat4 ---
static Mat4 mat4_multiply(Mat4 m, Mat4 n) {
    return (Mat4){
        .m00 = m.m00 * n.m00 + m.m01 * n.m10 + m.m02 * n.m20 + m.m03 * n.m30, .m01 = m.m00 * n.m01 + m.m01 * n.m11 + m.m02 * n.m21 + m.m03 * n.m31, .m02 = m.m00 * n.m02 + m.m01 * n.m12 + m.m02 * n.m22 + m.m03 * n.m32, .m03 = m.m00 * n.m03 + m.m01 * n.m13 + m.m02 * n.m23 + m.m03 * n.m33,
        .m10 = m.m10 * n.m00 + m.m11 * n.m10 + m.m12 * n.m20 + m.m13 * n.m30, .m11 = m.m10 * n.m01 + m.m11 * n.m11 + m.m12 * n.m21 + m.m13 * n.m31, .m12 = m.m10 * n.m02 + m.m11 * n.m12 + m.m12 * n.m22 + m.m13 * n.m32, .m13 = m.m10 * n.m03 + m.m11 * n.m13 + m.m12 * n.m23 + m.m13 * n.m33,
        .m20 = m.m20 * n.m00 + m.m21 * n.m10 + m.m22 * n.m20 + m.m23 * n.m30, .m21 = m.m20 * n.m01 + m.m21 * n.m11 + m.m22 * n.m21 + m.m23 * n.m31, .m22 = m.m20 * n.m02 + m.m21 * n.m12 + m.m22 * n.m22 + m.m23 * n.m32, .m23 = m.m20 * n.m03 + m.m21 * n.m13 + m.m22 * n.m23 + m.m23 * n.m33,
        .m30 = m.m30 * n.m00 + m.m31 * n.m10 + m.m32 * n.m20 + m.m33 * n.m30, .m31 = m.m30 * n.m01 + m.m31 * n.m11 + m.m32 * n.m21 + m.m33 * n.m31, .m32 = m.m30 * n.m02 + m.m31 * n.m12 + m.m32 * n.m22 + m.m33 * n.m32, .m33 = m.m30 * n.m03 + m.m31 * n.m13 + m.m32 * n.m23 + m.m33 * n.m33,
    };
}

static Mat4 mat4_transpose(Mat4 m) {
    return (Mat4){
        .m00 = m.m00, .m01 = m.m10, .m02 = m.m20, .m03 = m.m30,
        .m10 = m.m01, .m11 = m.m11, .m12 = m.m21, .m13 = m.m31,
        .m20 = m.m02, .m21 = m.m12, .m22 = m.m22, .m23 = m.m32,
        .m30 = m.m03, .m31 = m.m13, .m32 = m.m23, .m33 = m.m33,
    };
}

static Vec4 mat4_vec4_multiply(Mat4 m, Vec4 v) {
    return (Vec4){
        .x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w,
        .y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w,
        .z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w,
        .w = m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33 * v.w,
    };
}

// --- Quat ---
static Mat4 quat_to_rotation_mat4(Quat q) {
    f32 magnitude = quat_magnitude(q);
    f32 s = 1.0f / (magnitude * magnitude) ;
    return (Mat4){
        .m00 = 1.0f - 2.0f * s * (q.y * q.y + q.z * q.z),  .m01 = 2.0f * s * (q.x * q.y - q.z * q.w),        .m02 = 2.0f * s * (q.x * q.z + q.y * q.w),
        .m10 = 2.0f * s * (q.x * q.y + q.w * q.z),         .m11 = 1.0f - 2.0f * s * (q.x * q.x + q.z * q.z), .m12 = 2.0f * s * (q.y * q.z - q.w * q.x),
        .m20 = 2.0f * s * (q.x * q.z - q.w * q.y),         .m21 = 2.0f * s * (q.y * q.z + q.w * q.x),        .m22 = 1.0f - 2.0f * s * (q.x * q.x + q.y * q.y),
        .m33 = 1.0f,
    };
}

static Quat quat_conjugate(Quat q) {
    return (Quat){
        .w = q.w,
        .x = -q.x,
        .y = -q.y,
        .z = -q.z,
    };
}

static Quat quat_from_axis_angle(f32 angle, Vec3 axis) {
    Quat q = {0};
    f32 beta_x = vec3_angle_between(axis, vec3_ihat);
    f32 beta_y = vec3_angle_between(axis, vec3_jhat);
    f32 beta_z = vec3_angle_between(axis, vec3_khat);
    q.w = cosf(angle / 2.0f);
    q.x = sinf(angle / 2.0f)*cosf(beta_x);
    q.y = sinf(angle / 2.0f)*cosf(beta_y);
    q.z = sinf(angle / 2.0f)*cosf(beta_z);
    return quat_normalize(q);
}

static Quat quat_multiply(Quat q, Quat r) {
    return (Quat){
        .w = q.w * r.w - q.x * r.x - q.y * r.y - q.z * r.z,
        .x = q.w * r.x + q.x * r.w + q.y * r.z - q.z * r.y,
        .y = q.w * r.y - q.x * r.z + q.y * r.w + q.z * r.x,
        .z = q.w * r.z + q.x * r.y - q.y * r.x + q.z * r.w,
    };
}

static Quat quat_normalize(Quat q) {
    f32 factor = 1.0f / quat_magnitude(q);
    return (Quat){
        .w = q.w * factor,
        .x = q.x * factor,
        .y = q.y * factor,
        .z = q.z * factor,
    };
}

static f32 quat_magnitude(Quat q) {
        return sqrtf(q.a * q.a + q.b * q.b + q.c * q.c + q.d * q.d);
}
