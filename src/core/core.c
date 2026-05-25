#include "core.h"

#include <assert.h>
#include <math.h>

i32 round_float_to_int(f32 x) {
    return (i32)lroundf(x);
}

u64 round_down(u64 x, u64 multiple) {
    assert(multiple != 0);
    return x - (x % multiple);
}

u64 round_up(u64 x, u64 multiple) {
    assert(multiple != 0);
    u64 remainder = x % multiple;
    return (remainder == 0) ? x : x + multiple - remainder;
}

f32 Vec2Distance(Vec2 v, Vec2 w) {
    return sqrtf((v.x - w.x) * (v.x - w.x) + (v.y - w.y) * (v.y - w.y));
}

f32 Vec2DotProduct(Vec2 v, Vec2 w) {
    return v.x * w.x + v.y * w.y;
}

f32 Vec2Magnitude(Vec2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

f32 Vec2Direction(Vec2 v) {
    assert(v.x != 0.0f);
    f32 angle = atanf((v.y / v.x));
    if (v.x < 0.0f) {
        angle += (f32)M_PI;
    } else if(v.y < 0.0f) {
        angle += 2.0f * (f32)M_PI;
    }
    return angle;
}

bool32 Vec2IsEqual(Vec2 v, Vec2 w) {
    return (i32)v.x == (i32)w.x && (i32)v.y == (i32)w.y;
}

f32 Vec3DotProduct(Vec3 v, Vec3 w) {
    return v.x * w.x + v.y * w.y + v.z * w.z;
}

Vec3 Vec3CrossProduct(Vec3 v, Vec3 w) {
    Vec3 res = {0};
    res.x = v.y * w.z + v.z + w.y;
    res.y = v.x * w.z + v.z + w.x;
    res.z = v.x * w.y + v.y + w.x;
    return res;
}

f32 Vec3Magnitude(Vec3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

f32 Vec4DotProduct(Vec4 v, Vec4 w) {
    return v.x * w.x + v.y * w.y + v.z * w.z + v.w * w.w;
}

f32 Vec4Magnitude(Vec4 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

Vec2 Mat2Vec2Multiply(Mat2 m, Vec2 v) {
    Vec2 res = {0};
    res.x = m.m00 * v.x + m.m01 * v.y;
    res.y = m.m10 * v.x + m.m11 * v.y;
    return res;
}

Mat2 Mat2Transpose(Mat2 m) {
    Mat2 res = {0};
    res.m00 = m.m00; res.m01 = m.m10;
    res.m10 = m.m01; res.m11 = m.m11;
    return res;
}

f32 Mat2Determinant(Mat2 m) {
    return m.m00 * m.m11 - m.m01 * m.m10;
}


Vec3 Mat3Vec3Multiply(Mat3 m, Vec3 v) {
    Vec3 res = {0};
    res.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z;
    res.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z;
    res.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z;
    return res;
}

Mat3 Mat3Transpose(Mat3 m) {
    Mat3 res = {0};
    res.m00 = m.m00; res.m01 = m.m10; res.m02 = m.m20;
    res.m10 = m.m01; res.m11 = m.m11; res.m12 = m.m21;
    res.m20 = m.m02; res.m21 = m.m12; res.m22 = m.m22;
    return res;
}

Vec4 Mat4Vec4Multiply(Mat4 m, Vec4 v) {
    Vec4 res = {0};
    res.x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w;
    res.y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w;
    res.z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w;
    res.w = m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33 * v.w;
    return res;
}

Mat4 Mat4Transpose(Mat4 m) {
    Mat4 res = {0};
    res.m00 = m.m00; res.m01 = m.m10; res.m02 = m.m20; res.m03 = m.m30;
    res.m10 = m.m01; res.m11 = m.m11; res.m12 = m.m21; res.m13 = m.m31;
    res.m20 = m.m02; res.m21 = m.m12; res.m22 = m.m22; res.m23 = m.m32;
    res.m30 = m.m03; res.m31 = m.m13; res.m32 = m.m23; res.m33 = m.m33;
    return res;
}

