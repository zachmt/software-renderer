#include "render.h"
#include "core.h"

#include <stdio.h>
#include <stdlib.h>

static ColorRGBA red = {.r = 255, .g = 0, .b = 0, .a = 255};
static ColorRGBA green = {.r = 0, .g = 255, .b = 0, .a = 255};
static ColorRGBA blue = {.r = 0, .g = 0, .b = 255, .a = 255};
static ColorRGBA white = {.r = 255, .g = 255, .b = 255, .a = 255};
static ColorRGBA black = {.r = 0, .g = 0, .b = 0, .a = 255};

static void clear(RenderState *state, ColorRGBA color) {
    for (u32 i = 0; i < state->frame_buffer_len; i++) {
        state->frame_buffer[i] = color;
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

static f32 signed_triangle_area(Vec2 a, Vec2 b, Vec2 c) {
    // The determinant of this matrix computes the signed area of a
    // parallelogram formed by AB and AC. Multiply by 0.5 to get the area of
    // the triangle.
    Mat2 m = {0};
    m.m00 = b.x - a.x; m.m01 = c.x - a.x;
    m.m10 = b.y - a.y; m.m11 = c.y - a.y;
    return 0.5f * Mat2Determinant(m);
}

static void draw_triangle(RenderState *state, Vec2 a, Vec2 b, Vec2 c, ColorRGBA color) {
    Vec2 bounding_box[2] = {0};
    bounding_box[0].x = Min(Min(a.x, b.x), c.x);
    bounding_box[0].y = Min(Min(a.y, b.y), c.y);
    bounding_box[1].x = Max(Max(a.x, b.x), c.x);
    bounding_box[1].y = Max(Max(a.y, b.y), c.y);

    f32 total_area = signed_triangle_area(a, b, c);
    if (total_area < 1.0f) {
        // TODO: proper back-face culling
        // return;
    }
    for (i32 y = round_f32_to_i32(Min(bounding_box[0].y, bounding_box[1].y)); y <= round_f32_to_i32(Max(bounding_box[0].y, bounding_box[1].y)); y++) {
        for (i32 x = round_f32_to_i32(Min(bounding_box[0].x, bounding_box[1].x)); x <= round_f32_to_i32(Max(bounding_box[0].x, bounding_box[1].x)); x++) {
            Vec2 p = {
                .x = (f32)x,
                .y = (f32)y
            };
            f32 alpha = signed_triangle_area(p, b, c) / total_area;
            f32 beta  = signed_triangle_area(p, c, a) / total_area;
            f32 gamma = signed_triangle_area(p, a, b) / total_area;
            if (alpha < 0.0f || beta < 0.0f || gamma < 0.0f) {
                continue;
            }
            draw_pixel(state, x, y, color);
        }
    }
}

static Vec2 model_to_screen_projection(RenderState *state, Vec4 v) {
    f32 aspect_ratio = (f32)state->window_width / (f32)state->window_height;
    Vec2 res = {0};
    res.x = (v.x + 1.0f) * (f32)state->window_width * 0.5f;
    res.y = (-v.y + 1.0f) * (f32)state->window_height * 0.5f * aspect_ratio;
    return res;
}

static void draw_wireframe(RenderState *state, Model *mesh, ColorRGBA color) {
    for (u32 face_index = 0; face_index < mesh->face_count; face_index++) {
        Face *face = mesh->faces + face_index;
        Vec2 vert0 = model_to_screen_projection(state, mesh->vertices[face->vertex_indices[0]]);
        Vec2 vert1 = model_to_screen_projection(state, mesh->vertices[face->vertex_indices[1]]);
        Vec2 vert2 = model_to_screen_projection(state, mesh->vertices[face->vertex_indices[2]]);
        draw_line(state, vert0, vert1, color);
        draw_line(state, vert1, vert2, color);
        draw_line(state, vert2, vert0, color);
    }
}

static ColorRGBA random_color(void) {
    ColorRGBA res = {0};
    res.r = (u8)rand();
    res.g = (u8)rand();
    res.b = (u8)rand();
    res.a = 255;
    return res;
}

static void draw_model(RenderState *state, Model *mesh) {
    srand(0);
    for (u32 face_index = 0; face_index < mesh->face_count; face_index++) {
        Face *face = mesh->faces + face_index;
        Vec2 vert0 = model_to_screen_projection(state, mesh->vertices[face->vertex_indices[0]]);
        Vec2 vert1 = model_to_screen_projection(state, mesh->vertices[face->vertex_indices[1]]);
        Vec2 vert2 = model_to_screen_projection(state, mesh->vertices[face->vertex_indices[2]]);
        draw_triangle(state, vert0, vert1, vert2, random_color());
    }
}




static void update_and_render(RenderState *state) {
    Assert(state->window_width * state->window_height <= state->frame_buffer_len);

    Object obj = {0};
    obj.model    = state->model;
    obj.scale = 1.0f;
    obj.position = (Vec3){.x = 0.0f, .y = 0.0f, .z = 0.0f};
    obj.rotation = QuatIdentity();

    Camera cam = {0};
    cam.position = (Vec3){.x = 0.5f, .y = -0.5f, .z = -30.0f};
    cam.rotation = QuatIdentity();
    cam.fov_y_radians = PI32/2.0f;
    cam.near_clip = 0.1f;
    cam.far_clip = 100.0f;

    clear(state, black);
    // Vec2 a = { .x = 1, .y = 1 };
    // Vec2 b = { .x = 30, .y = 30 };
    // Vec2 c = { .x = 60, .y = 60 };
    //
    // Vec2 d = { .x = 120, .y = 35 };
    // Vec2 e = { .x = 90, .y = 5 };
    // Vec2 f = { .x = 45, .y = 110 };
    //
    // Vec2 g = { .x = 115, .y = 83 };
    // Vec2 h = { .x = 80, .y = 90 };
    // Vec2 i = { .x = 85, .y = 120 };

    // draw_triangle(state, a, b, c, red);
    // draw_triangle(state, d, e, f, green);
    // draw_triangle(state, g, h, i, blue);
    // draw_triangle(state, a, a, a, white);
    draw_model(state, state->model);
}

