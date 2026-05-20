#include "render.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
        u32 i = (y * state->window_width + x);
        state->frame_buffer[i] = color;
    }
}

static void draw_line(RenderState *state, i32 ax, i32 ay, i32 bx, i32 by, ColorRGBA color) {
    bool32 steep = abs(ax-bx) < abs(ay-by);
    if (steep) {
        SWAP(i32, ax, ay);
        SWAP(i32, bx, by);
    }
    if (ax > bx) {
        SWAP(i32, ax, bx);
        SWAP(i32, ay, by);
    }
    for (i32 x = ax; x < bx; x++) {
        float t = ((float)(x-ax)) / ((float)(bx-ax));
        i32 y = (u32)roundf(((float)ay + ((float)(by-ay)) * t));
        if (steep) {
            draw_pixel(state, y, x, color);
        } else {
            draw_pixel(state, x, y, color);
        }
    }
}

static Vec2 model_to_screen_projection(RenderState *state, Vec4 v) {
    f32 aspect_ratio = (f32)state->window_width / (f32)state->window_height;
    Vec2 res = {0};
    res.x = (v.x + 1.0) * (f32)state->window_width * 0.5;
    res.y = (v.y - 1.0) * (f32)state->window_height * 0.5 * -1 * aspect_ratio;
    return res;
}

static void draw_wireframe(RenderState *state, Model *mesh, ColorRGBA color) {
    for (u32 face_index = 0; face_index < mesh->face_count; face_index++) {
        Face *face = mesh->faces + face_index;
        Vec2 vert0 = model_to_screen_projection(state, mesh->vertices[face->vertex_indices[0]]);
        Vec2 vert1 = model_to_screen_projection(state, mesh->vertices[face->vertex_indices[1]]);
        Vec2 vert2 = model_to_screen_projection(state, mesh->vertices[face->vertex_indices[2]]);
        draw_line(state, vert0.x, vert0.y, vert1.x, vert1.y, color);
        draw_line(state, vert1.x, vert1.y, vert2.x, vert2.y, color);
        draw_line(state, vert2.x, vert2.y, vert0.x, vert0.y, color);
    }
}

void update_and_render(RenderState *state) {
    assert(state->window_width * state->window_height <= state->frame_buffer_len);

    clear(state, black);
    draw_wireframe(state, state->model, white);
}
