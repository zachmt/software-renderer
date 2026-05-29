#include "render.h"
#include "core.h"
#include "maths.h"
#include "stdio.h"

static ColorRGBA red = {.r = 255, .g = 0, .b = 0, .a = 255};
static ColorRGBA green = {.r = 0, .g = 255, .b = 0, .a = 255};
static ColorRGBA blue = {.r = 0, .g = 0, .b = 255, .a = 255};
static ColorRGBA white = {.r = 255, .g = 255, .b = 255, .a = 255};
static ColorRGBA black = {.r = 0, .g = 0, .b = 0, .a = 255};

static ColorRGBA random_color(void) {
    ColorRGBA res = {0};
    res.r = (u8)rng_generate_i32();
    res.g = (u8)rng_generate_i32();
    res.b = (u8)rng_generate_i32();
    res.a = 255;
    return res;
}

static void clear_frame_buffer(RenderState *state, ColorRGBA color) {
    for (u32 i = 0; i < state->frame_buffer_len; i++) {
        state->frame_buffer[i] = color;
    }
}

static void reset_depth_buffer(RenderState *state) {
    for (u32 i = 0; i < state->depth_buffer_len; i++) {
        state->depth_buffer[i] = inf32();
    }
}

static void draw_pixel(RenderState *state, i32 x, i32 y, ColorRGBA color) {
    if (x >= 0 && x < (i32)state->window_width && y >= 0 && y < (i32)state->window_height) {
        u32 i = ((u32)y * state->window_width + (u32)x);
        state->frame_buffer[i] = color;
    }
}

static void draw_line(RenderState *state, Vec2 a, Vec2 b, ColorRGBA color) {
    bool32 steep = f32_abs(a.x-b.x) < f32_abs(a.y-b.y);
    if (steep) {
        Swap(f32, a.x, a.y);
        Swap(f32, b.x, b.y);
    }
    if (a.x > b.x) {
        Swap(f32, a.x, b.x);
        Swap(f32, a.y, b.y);
    }
    for (i32 x = round_f32_to_i32(a.x); x < round_f32_to_i32(b.x); x++) {
        f32 t = (((f32)x-a.x)) / ((b.x-a.x));
        i32 y = round_f32_to_i32(a.y + (b.y-a.y) * t);
        if (steep) {
            draw_pixel(state, y, x, color);
        } else {
            draw_pixel(state, x, y, color);
        }
    }
}

static f32 signed_triangle_area(Vec3 a, Vec3 b, Vec3 c) {
    // The determinant of this matrix computes the signed area of a
    // parallelogram formed by AB and AC. Multiply by 0.5 to get the area of
    // the triangle.
    Mat2 m = {0};
    m.m00 = b.x - a.x; m.m01 = c.x - a.x;
    m.m10 = b.y - a.y; m.m11 = c.y - a.y;
    return 0.5f * Mat2Determinant(m);
}

static void draw_triangle(RenderState *state, Vec3 a, Vec3 b, Vec3 c, ColorRGBA color) {
    Vec2 bounding_box[2] = {0};
    bounding_box[0].x = Min(Min(a.x, b.x), c.x);
    bounding_box[0].y = Min(Min(a.y, b.y), c.y);
    bounding_box[1].x = Max(Max(a.x, b.x), c.x);
    bounding_box[1].y = Max(Max(a.y, b.y), c.y);

    f32 total_area = signed_triangle_area(a, b, c);
    for (i32 y = round_f32_to_i32(Min(bounding_box[0].y, bounding_box[1].y)); y <= round_f32_to_i32(Max(bounding_box[0].y, bounding_box[1].y)); y++) {
        for (i32 x = round_f32_to_i32(Min(bounding_box[0].x, bounding_box[1].x)); x <= round_f32_to_i32(Max(bounding_box[0].x, bounding_box[1].x)); x++) {
            Vec3 p = {
                .x = (f32)x,
                .y = (f32)y,
                .z = 0.0f,
            };
            f32 alpha = signed_triangle_area(p, b, c) / total_area;
            f32 beta  = signed_triangle_area(p, c, a) / total_area;
            f32 gamma = signed_triangle_area(p, a, b) / total_area;
            if (!(alpha < 0.0f || beta < 0.0f || gamma < 0.0f)) {
                p.z = alpha * a.z + beta * b.z + gamma * c.z;
                u32 depth_index = (u32)y * state->window_width + (u32)x;
                Assert(depth_index < state->depth_buffer_len);
                if (p.z < state->depth_buffer[depth_index]) {
                    state->depth_buffer[depth_index] = p.z;
                    draw_pixel(state, x, y, color);
                }
            }
        }
    }
}

static Mat4 get_model_to_world_matrix(Object *obj) {
    Mat4 scale = {0};
    scale.m00 = obj->scale;
    scale.m11 = obj->scale;
    scale.m22 = obj->scale;
    scale.m33 = 1.0f;

    Mat4 translate = {0};
    translate.m00 = 1.0f; translate.m03 = obj->position.x;
    translate.m11 = 1.0f; translate.m13 = obj->position.y;
    translate.m22 = 1.0f; translate.m23 = obj->position.z;
    translate.m33 = 1.0f;

    Mat4 rotate = QuatToRotationMat4(obj->rotation);
    // rotate = Mat4Identity();

    return Mat4Multiply(translate, Mat4Multiply(rotate, scale));
}

static Mat4 get_world_to_view_matrix(Camera *cam) {
    Mat4 translate = {0};
    translate.m00 = 1.0f; translate.m03 = -cam->position.x;
    translate.m11 = 1.0f; translate.m13 = -cam->position.y;
    translate.m22 = 1.0f; translate.m23 = -cam->position.z;
    translate.m33 = 1.0f;

    Mat4 rotate = QuatToRotationMat4(QuatConjugate(cam->rotation));
    // rotate = Mat4Identity();

    Mat4 basis_change = {0};
    basis_change.m00 = 1.0f;
    basis_change.m12 = 1.0f;
    basis_change.m21 = 1.0f;
    basis_change.m33 = 1.0f;

    return Mat4Multiply(basis_change, Mat4Multiply(rotate, translate));
}

static Mat4 get_view_to_clip_matrix(Camera *cam) {
    f32 top = cam->near_clip * tangent(cam->fov_y_radians / 2.0f);
    f32 right = top * cam->aspect_ratio;
    Mat4 projection = {0};
    projection.m00 = cam->near_clip / right;
    projection.m11 = cam->near_clip / top;
    projection.m22 = (cam->far_clip + cam->near_clip) / (cam->far_clip - cam->near_clip);
    projection.m23 = (-2.0f * cam->far_clip * cam->near_clip) / (cam->far_clip - cam->near_clip);
    projection.m32 = 1.0f;
    return projection;
}

static Vec4 ndc_to_screen(RenderState *state, Vec4 ndc) {
    Vec4 res = {0};
    res.x = (ndc.x * 0.5f + 0.5f) * (f32)state->window_width;
    res.y = (-ndc.y * 0.5f + 0.5f) * (f32)state->window_height;
    res.z = ndc.z;
    res.w = 1.0f;
    return res;
}

// Model Space (Vec4) -> World Space (Vec4) -> View Space (Vec4) -> Clip Space (Vec4) -> NDC (Vec3) -> Screen Space (Vec3)
static void draw_object(RenderState *state, Object *obj) {
    rng_seed(0);
    Mat4 model_to_world = get_model_to_world_matrix(obj);

    Mat4 world_to_view = get_world_to_view_matrix(&state->camera);
    Mat4 view_to_clip = get_view_to_clip_matrix(&state->camera);

    Mat4 world_to_clip = Mat4Multiply(view_to_clip, world_to_view);
    Mat4 model_to_clip = Mat4Multiply(world_to_clip, model_to_world);

    for (u32 face_index = 0; face_index < obj->model->face_count; face_index++) {
        Face *face = obj->model->faces + face_index;
        Vec4 A = obj->model->vertices[face->vertex_indices[0]];
        Vec4 B = obj->model->vertices[face->vertex_indices[1]];
        Vec4 C = obj->model->vertices[face->vertex_indices[2]];

        A = Mat4Vec4Multiply(model_to_clip, A);
        B = Mat4Vec4Multiply(model_to_clip, B);
        C = Mat4Vec4Multiply(model_to_clip, C);

        // TODO: better clipping
        if (A.x > -A.w && A.x < A.w &&
                A.y > -A.w && A.y < A.w &&
                A.z > -A.w && A.z < A.w &&
                B.x > -B.w && B.x < B.w &&
                B.y > -B.w && B.y < B.w &&
                B.z > -B.w && B.z < B.w &&
                C.x > -C.w && C.x < C.w &&
                C.y > -C.w && C.y < C.w &&
                C.z > -C.w && C.z < C.w) {
            A = Vec4Scale(A, 1.0f / A.w);
            B = Vec4Scale(B, 1.0f / B.w);
            C = Vec4Scale(C, 1.0f / C.w);
            A = ndc_to_screen(state, A);
            B = ndc_to_screen(state, B);
            C = ndc_to_screen(state, C);

            Vec3 a, b, c;
            a.x = A.x; b.x = B.x; c.x = C.x;
            a.y = A.y; b.y = B.y; c.y = C.y;
            a.z = A.z; b.z = B.z; c.z = C.z;
            draw_triangle(state, a, b, c, random_color());
        }
    }
}


static void update_and_render(RenderState *state) {
    const f32 max_camera_speed = 1.0f;
    Assert(state->window_width * state->window_height <= state->frame_buffer_len);
    printf("left_right %f\n", state->controls.left_right);
    printf("forward_backward %f\n", state->controls.forward_backward);
    state->camera.aspect_ratio = (f32)state->window_width / (f32)state->window_height;
    state->camera.position.x += 0.01 * state->controls.left_right;
    state->camera.position.y += 0.01 * state->controls.forward_backward;
    state->camera.position.z += 0.01 * state->controls.up_down;


    clear_frame_buffer(state, black);
    reset_depth_buffer(state);
    draw_object(state, state->obj);
}

