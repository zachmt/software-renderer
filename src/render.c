#include "render.h"
#include "core.h"
#include "maths.h"

#include <stdio.h>
#include <time.h>

#include "stb_easy_font.h"

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
        state->depth_buffer[i] = f32_neg_inf();
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
        swap(f32, a.x, a.y);
        swap(f32, b.x, b.y);
    }
    if (a.x > b.x) {
        swap(f32, a.x, b.x);
        swap(f32, a.y, b.y);
    }
    for (i32 x = f32_round_to_i32(a.x); x < f32_round_to_i32(b.x); x++) {
        f32 t = (((f32)x-a.x)) / ((b.x-a.x));
        i32 y = f32_round_to_i32(a.y + (b.y-a.y) * t);
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
    return 0.5f * mat2_determinant(m);
}

static void draw_triangle(RenderState *state, Vec3 a, Vec3 b, Vec3 c, ColorRGBA color) {
    i32 minx, miny, maxx, maxy;
    minx = f32_round_to_i32(min(min(a.x, b.x), c.x));
    miny = f32_round_to_i32(min(min(a.y, b.y), c.y));
    maxx = f32_round_to_i32(max(max(a.x, b.x), c.x));
    maxy = f32_round_to_i32(max(max(a.y, b.y), c.y));

    f32 total_area = signed_triangle_area(a, b, c);
    for (i32 y = max(0, miny); y <= min((i32)state->window_height-1, maxy); y++) {
        for (i32 x = max(0, minx); x <= min((i32)state->window_width-1, maxx); x++) {
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
                runtime_assert(depth_index < state->depth_buffer_len);
                if (p.z > state->depth_buffer[depth_index]) {
                    state->depth_buffer[depth_index] = p.z;
                    draw_pixel(state, x, y, color);
                }
            }
        }
    }
}

static Mat4 get_model_to_world_mat4(Object *obj) {
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

    Mat4 rotate = quat_to_rotation_mat4(obj->rotation);

    return mat4_multiply(translate, mat4_multiply(rotate, scale));
}

static Mat4 get_world_to_view_mat4(Camera *cam) {
    Mat4 translate = {0};
    translate.m00 = 1.0f; translate.m03 = -cam->position.x;
    translate.m11 = 1.0f; translate.m13 = -cam->position.y;
    translate.m22 = 1.0f; translate.m23 = -cam->position.z;
    translate.m33 = 1.0f;

    Mat4 rotate = quat_to_rotation_mat4(quat_conjugate(cam->rotation));

    Mat4 basis_change = {0};
    basis_change.m00 = 1.0f;
    basis_change.m12 = -1.0f;
    basis_change.m21 = 1.0f;
    basis_change.m33 = 1.0f;

    return mat4_multiply(basis_change, mat4_multiply(rotate, translate));
}

static Mat4 get_view_to_clip_mat4(Camera *cam) {
    f32 top = cam->near_clip * f32_tan(cam->fov_y_radians / 2.0f);
    f32 right = top * cam->aspect_ratio;
    Mat4 projection = {0};
    projection.m00 = cam->near_clip / right;
    projection.m11 = cam->near_clip / top;
    projection.m22 = (cam->far_clip + cam->near_clip) / (cam->far_clip - cam->near_clip);
    projection.m23 = (2.0f * cam->far_clip * cam->near_clip) / (cam->far_clip - cam->near_clip);
    projection.m32 = -1.0f;
    return projection;
}

static Vec3 ndc_to_screen(RenderState *state, Vec4 ndc) {
    return (Vec3){
        .x = (ndc.x * 0.5f + 0.5f) * (f32)state->window_width,
        .y = (-ndc.y * 0.5f + 0.5f) * (f32)state->window_height,
        .z = ndc.z,
    };
}

static void rasterize_triangle(RenderState *state, Vec4 A, Vec4 B, Vec4 C) { // TODO: add fragment shader function pointer?
    ColorRGBA color = random_color();

    if (
            A.x < A.w && A.x > -A.w &&
            A.y < A.w && A.y > -A.w &&
            A.z < A.w && A.z > -A.w &&
            B.x < B.w && B.x > -B.w &&
            B.y < B.w && B.y > -B.w &&
            B.z < B.w && B.z > -B.w &&
            C.x < C.w && C.x > -C.w &&
            C.y < C.w && C.y > -C.w &&
            C.z < C.w && C.z > -C.w
       ) {

        // Clip -> NDC
        A = vec4_scale(A, 1.0f / A.w);
        B = vec4_scale(B, 1.0f / B.w);
        C = vec4_scale(C, 1.0f / C.w);


        // NDC -> screen
        Vec3 a = ndc_to_screen(state, A);
        Vec3 b = ndc_to_screen(state, B);
        Vec3 c = ndc_to_screen(state, C);


        draw_triangle(state, a, b, c, color);
    }
}

// Model Space (Vec4) -> World Space (Vec4) -> View Space (Vec4) -> Clip Space (Vec4) -> NDC (Vec3) -> Screen Space (Vec3)
static void draw_object(RenderState *state, Object *obj) {
    rng_seed(0);
    Mat4 model_to_world = get_model_to_world_mat4(obj);

    Mat4 world_to_view = get_world_to_view_mat4(&state->camera);
    Mat4 view_to_clip = get_view_to_clip_mat4(&state->camera);

    Mat4 world_to_clip = mat4_multiply(view_to_clip, world_to_view);
    Mat4 model_to_clip = mat4_multiply(world_to_clip, model_to_world);

    for (u32 face_index = 0; face_index < obj->model->face_count; face_index++) {
        Face *face = obj->model->faces + face_index;
        Vec4 A = obj->model->mesh_vertices[face->vertex_indices[0]];
        Vec4 B = obj->model->mesh_vertices[face->vertex_indices[1]];
        Vec4 C = obj->model->mesh_vertices[face->vertex_indices[2]];

        A = mat4_vec4_multiply(model_to_clip, A);
        B = mat4_vec4_multiply(model_to_clip, B);
        C = mat4_vec4_multiply(model_to_clip, C);

        rasterize_triangle(state, A, B,C);
    }
}

static void render_2d_text(RenderState *state, Vec2 offset, char *str) {
    EasyFontVertex vb[1000 * sizeof(EasyFontVertex) * 4];
    stb_easy_font_spacing(-0.5f);
    i32 num_quads = stb_easy_font_print(offset.x, offset.y, str, NULL, vb, sizeof(vb));
    const f32 scale = 2.0f;
    for (i32 i = 0; i < num_quads * 4; i+=4) {
        Vec3 a = {
            .x = vb[i].x * scale,
            .y = vb[i].y * scale,
            .z = 100.0f,
        };
        Vec3 b = {
            .x = vb[i+1].x * scale,
            .y = vb[i+1].y * scale,
            .z = 100.0f,
        };
        Vec3 c = {
            .x = vb[i+2].x * scale,
            .y = vb[i+2].y * scale,
            .z = 100.0f,
        };
        Vec3 d = {
            .x = vb[i+3].x * scale,
            .y = vb[i+3].y * scale,
            .z = 100.0f,
        };
        draw_triangle(state, a, b, c, white);
        draw_triangle(state, c, d, a, white);
    }
}

static void update_camera(RenderState *state, f32 dt) {
    state->camera.aspect_ratio = (f32)state->window_width / (f32)state->window_height;
    f32 movement_speed = 10.0f; // meters per second
    Vec4 view_delta = {
        .x = state->controls.left_right,
        .y = state->controls.up_down,
        .z = -state->controls.forward_backward,
        .w = 0.0f,
    };

    if (!vec4_is_equal(view_delta, vec4_zero)) {
        view_delta = vec4_scale(vec4_normalize(view_delta), movement_speed * dt);
    }

    Mat4 view_to_world_rotate = quat_to_rotation_mat4(state->camera.rotation);
    Mat4 basis_change = {0};
    basis_change.m00 = 1.0f;
    basis_change.m12 = 1.0f;
    basis_change.m21 = -1.0f;
    basis_change.m33 = 1.0f;
    Mat4 view_to_world = mat4_multiply(view_to_world_rotate, basis_change);
    Vec4 world_delta = mat4_vec4_multiply(view_to_world, view_delta);
    state->camera.position = vec4_add(state->camera.position, world_delta);


    f32 roll_input = state->controls.look_roll;
    f32 pitch_input = state->controls.look_pitch;
    f32 yaw_input = state->controls.look_yaw;
    f32 roll_speed = 2.0f * f32_pi; // radians per second
    f32 pitch_speed = 2.0f * f32_pi; // radians per second
    f32 yaw_speed = 2.0f * f32_pi; // radians per second
    
    f32 roll_angle = roll_input * roll_speed * dt;
    f32 pitch_angle = pitch_input * pitch_speed * dt;
    f32 yaw_angle = yaw_input * yaw_speed * dt;

    // Roll in local camera space (not world space)
    Quat roll_delta = quat_from_axis_angle(roll_angle, vec3_jhat);
    Quat pitch_delta = quat_from_axis_angle(pitch_angle, vec3_ihat);
    Quat yaw_delta = quat_from_axis_angle(yaw_angle, vec3_khat);

    Quat delta = quat_multiply(yaw_delta, quat_multiply(pitch_delta, roll_delta));

    state->camera.rotation = quat_normalize(quat_multiply(state->camera.rotation, delta));
}

static void update_and_render(RenderState *state, f32 dt) {
    runtime_assert(state->window_width * state->window_height <= state->frame_buffer_len);
    update_camera(state, dt);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC,&start);
    clear_frame_buffer(state, black);
    reset_depth_buffer(state);
    draw_object(state, state->obj);
    clock_gettime(CLOCK_MONOTONIC,&end);

    i64 s = end.tv_sec - start.tv_sec;
    i64 ns = end.tv_nsec - start.tv_nsec;
    f64 ms = (f64)s * 1000.0 + 0.000001 * (f64)ns;

    char debug_text[1000] = {0};
    snprintf(debug_text, 1000, "dt %f s\nCamera pos (%f, %f, %f)\nCamera rot (%f, %f, %f, %f)\n%f ms", (f64)dt, (f64)state->camera.position.x, (f64)state->camera.position.y, (f64)state->camera.position.z, (f64)state->camera.rotation.w, (f64)state->camera.rotation.x, (f64)state->camera.rotation.y, (f64)state->camera.rotation.z, ms);
    render_2d_text(state, vec2_zero, debug_text);
}
