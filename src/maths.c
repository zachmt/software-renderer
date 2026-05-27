#include "maths.h"
#include <math.h>
#include <stdlib.h>

static i32 i32_abs(i32 x) {
    return abs(x);
}
static f32 f32_abs(f32 x) {
    return fabsf(x);
}

static i32 round_f32_to_i32(f32 x) {
    return (i32)lroundf(x);
}

static u64 round_down(u64 x, u64 multiple) {
    Assert(multiple != 0);
    return x - (x % multiple);
}

static u64 round_up(u64 x, u64 multiple) {
    Assert(multiple != 0);
    u64 remainder = x % multiple;
    return (remainder == 0) ? x : x + multiple - remainder;
}

// --- Vec2 ---
static Vec2 Vec2Normalize(Vec2 v) {
    return Vec2Scale(v, 1.0f / Vec2Magnitude(v));
}

static Vec2 Vec2Scale(Vec2 v, f32 n) {
    Vec2 res = v;
    res.x *= n;
    res.y *= n;
    return res;
}

static bool32 Vec2IsEqual(Vec2 v, Vec2 w) {
    return v.x == w.x && v.y == w.y;
}

static f32 Vec2Direction(Vec2 v) {
    Assert(v.x != 0.0f);
    f32 angle = atanf((v.y / v.x));
    if (v.x < 0.0f) {
        angle += (f32)M_PI;
    } else if(v.y < 0.0f) {
        angle += 2.0f * (f32)M_PI;
    }
    return angle;
}

static f32 Vec2Distance(Vec2 v, Vec2 w) {
    return sqrtf((v.x - w.x) * (v.x - w.x) + (v.y - w.y) * (v.y - w.y));
}

static f32 Vec2DotProduct(Vec2 v, Vec2 w) {
    return v.x * w.x + v.y * w.y;
}

static f32 Vec2Magnitude(Vec2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

// --- Vec3 ---
static Vec3 Vec3CrossProduct(Vec3 v, Vec3 w) {
    Vec3 res = {0};
    res.x = v.y * w.z + v.z + w.y;
    res.y = v.x * w.z + v.z + w.x;
    res.z = v.x * w.y + v.y + w.x;
    return res;
}

static Vec3 Vec3Normalize(Vec3 v) {
    return Vec3Scale(v, 1.0f / Vec3Magnitude(v));
}

static Vec3 Vec3Scale(Vec3 v, f32 n) {
    Vec3 res = v;
    res.x *= n;
    res.y *= n;
    res.z *= n;
    return res;
}

static bool32 Vec3IsEqual(Vec3 v, Vec3 w) {
    return v.x == w.x && v.y == w.y && v.z == w.z;
}


static f32 Vec3DotProduct(Vec3 v, Vec3 w) {
    return v.x * w.x + v.y * w.y + v.z * w.z;
}

static f32 Vec3Magnitude(Vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// --- Vec4 ---
static Vec4 Vec4Normalize(Vec4 v) {
    return Vec4Scale(v, 1.0f / Vec4Magnitude(v));
}

static Vec4 Vec4Scale(Vec4 v, f32 n) {
    Vec4 res = v;
    res.x *= n;
    res.y *= n;
    res.z *= n;
    res.w *= n;
    return res;
}

static bool32 Vec4IsEqual(Vec4 v, Vec4 w) {
    return v.x == w.x && v.y == w.y && v.z == w.z && v.w == w.w;
}

static f32 Vec4DotProduct(Vec4 v, Vec4 w) {
    return v.x * w.x + v.y * w.y + v.z * w.z + v.w * w.w;
}

static f32 Vec4Magnitude(Vec4 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

// --- Mat2 ---
static Mat2 Mat2Identity(void) {
    Mat2 res = {0};
    res.m00 = 1.0f;
    res.m11 = 1.0f;
    return res;
}

static Mat2 Mat2Multiply(Mat2 m, Mat2 n) {
    Mat2 res = {0};
    res.m00 = m.m00 * n.m00 + m.m01 * n.m10; res.m01 = m.m00 * n.m01 + m.m01 * n.m11;
    res.m10 = m.m10 * n.m00 + m.m11 * n.m10; res.m11 = m.m10 * n.m01 + m.m11 * n.m11;
    return res;
}

static Mat2 Mat2Transpose(Mat2 m) {
    Mat2 res = {0};
    res.m00 = m.m00; res.m01 = m.m10;
    res.m10 = m.m01; res.m11 = m.m11;
    return res;
}

static Vec2 Mat2Vec2Multiply(Mat2 m, Vec2 v) {
    Vec2 res = {0};
    res.x = m.m00 * v.x + m.m01 * v.y;
    res.y = m.m10 * v.x + m.m11 * v.y;
    return res;
}

static f32 Mat2Determinant(Mat2 m) {
    return m.m00 * m.m11 - m.m01 * m.m10;
}

// --- Mat3 ---
static Mat3 Mat3Identity(void) {
    Mat3 res = {0};
    res.m00 = 1.0f;
    res.m11 = 1.0f;
    res.m22 = 1.0f;
    return res;
}

static Mat3 Mat3Multiply(Mat3 m, Mat3 n) {
    Mat3 res = {0};
    res.m00 = m.m00 * n.m00 + m.m01 * n.m10 + m.m02 * n.m20; res.m01 = m.m00 * n.m01 + m.m01 * n.m11 + m.m02 * n.m21; res.m02 = m.m00 * n.m02 + m.m01 * n.m12 + m.m02 * n.m22;
    res.m10 = m.m10 * n.m00 + m.m11 * n.m10 + m.m12 * n.m20; res.m11 = m.m10 * n.m01 + m.m11 * n.m11 + m.m12 * n.m21; res.m12 = m.m10 * n.m02 + m.m11 * n.m12 + m.m12 * n.m22;
    res.m20 = m.m20 * n.m00 + m.m21 * n.m10 + m.m22 * n.m20; res.m21 = m.m20 * n.m01 + m.m21 * n.m11 + m.m22 * n.m21; res.m22 = m.m20 * n.m02 + m.m21 * n.m12 + m.m22 * n.m22;
    return res;
}

static Mat3 Mat3Transpose(Mat3 m) {
    Mat3 res = {0};
    res.m00 = m.m00; res.m01 = m.m10; res.m02 = m.m20;
    res.m10 = m.m01; res.m11 = m.m11; res.m12 = m.m21;
    res.m20 = m.m02; res.m21 = m.m12; res.m22 = m.m22;
    return res;
}

static Vec3 Mat3Vec3Multiply(Mat3 m, Vec3 v) {
    Vec3 res = {0};
    res.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z;
    res.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z;
    res.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z;
    return res;
}

// --- Mat4 ---
static Mat4 Mat4Identity(void) {
    Mat4 res = {0};
    res.m00 = 1.0f;
    res.m11 = 1.0f;
    res.m22 = 1.0f;
    res.m33 = 1.0f;
    return res;
}

static Mat4 Mat4Multiply(Mat4 m, Mat4 n) {
    Mat4 res = {0};
    res.m00 = m.m00 * n.m00 + m.m01 * n.m10 + m.m02 * n.m20 + m.m03 * n.m30; res.m01 = m.m00 * n.m01 + m.m01 * n.m11 + m.m02 * n.m21 + m.m03 * n.m31; res.m02 = m.m00 * n.m02 + m.m01 * n.m12 + m.m02 * n.m22 + m.m03 * n.m32; res.m03 = m.m00 * n.m03 + m.m01 * n.m13 + m.m02 * n.m23 + m.m03 * n.m33;
    res.m10 = m.m10 * n.m00 + m.m11 * n.m10 + m.m12 * n.m20 + m.m13 * n.m30; res.m11 = m.m10 * n.m01 + m.m11 * n.m11 + m.m12 * n.m21 + m.m13 * n.m31; res.m12 = m.m10 * n.m02 + m.m11 * n.m12 + m.m12 * n.m22 + m.m13 * n.m32; res.m13 = m.m10 * n.m03 + m.m11 * n.m13 + m.m12 * n.m23 + m.m13 * n.m33;
    res.m20 = m.m20 * n.m00 + m.m21 * n.m10 + m.m22 * n.m20 + m.m23 * n.m30; res.m21 = m.m20 * n.m01 + m.m21 * n.m11 + m.m22 * n.m21 + m.m23 * n.m31; res.m22 = m.m20 * n.m02 + m.m21 * n.m12 + m.m22 * n.m22 + m.m23 * n.m32; res.m23 = m.m20 * n.m03 + m.m21 * n.m13 + m.m22 * n.m23 + m.m23 * n.m33;
    res.m30 = m.m30 * n.m00 + m.m31 * n.m10 + m.m32 * n.m20 + m.m33 * n.m30; res.m31 = m.m30 * n.m01 + m.m31 * n.m11 + m.m32 * n.m21 + m.m33 * n.m31; res.m32 = m.m30 * n.m02 + m.m31 * n.m12 + m.m32 * n.m22 + m.m33 * n.m32; res.m33 = m.m30 * n.m03 + m.m31 * n.m13 + m.m32 * n.m23 + m.m33 * n.m33;
    return res;
}


static Mat4 Mat4Transpose(Mat4 m) {
    Mat4 res = {0};
    res.m00 = m.m00; res.m01 = m.m10; res.m02 = m.m20; res.m03 = m.m30;
    res.m10 = m.m01; res.m11 = m.m11; res.m12 = m.m21; res.m13 = m.m31;
    res.m20 = m.m02; res.m21 = m.m12; res.m22 = m.m22; res.m23 = m.m32;
    res.m30 = m.m03; res.m31 = m.m13; res.m32 = m.m23; res.m33 = m.m33;
    return res;
}

static Vec4 Mat4Vec4Multiply(Mat4 m, Vec4 v) {
    Vec4 res = {0};
    res.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w;
    res.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w;
    res.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w;
    res.w = m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33 * v.w;
    return res;
}

// --- Quat ---
static Mat4 QuatToMat4(Quat q) {
    Mat4 res = {0};
    res.m00 = 1.0f - 2.0f * (q.c * q.c + q.d * q.d); res.m01 = 2.0f * (q.b * q.c - q.d * q.a);        res.m02 = 2.0f * (q.b * q.d + q.c * q.a);
    res.m10 = 2.0f * (q.b * q.c + q.a * q.d);        res.m11 = 1.0f - 2.0f * (q.b * q.b + q.d * q.d); res.m12 = 2.0f * (q.c * q.d - q.a * q.b);
    res.m20 = 2.0f * (q.b * q.d + q.a * q.c);        res.m21 = 2.0f * (q.c * q.d + q.a * q.b);        res.m22 = 1.0f - 2.0f * (q.b * q.b + q.c * q.b);
    res.m33 = 1.0f;
    return res;
}

static Quat QuatConjugate(Quat q) {
    Quat res = {0};
    res.a = q.a;
    res.b = -q.b;
    res.c = -q.c;
    res.d = -q.d;
    return res;
}

static Quat QuatIdentity(void) {
    Quat res = {0};
    res.d = 1.0f;
    return res;
}
