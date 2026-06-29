#include "render.h"
#include "core.h"
#include "maths.h"

#include <stdio.h>

#include "os.h"
#include "stb_easy_font.h"

static ColorRGBA red = {.r = 255, .g = 0, .b = 0, .a = 255};
static ColorRGBA green = {.r = 0, .g = 255, .b = 0, .a = 255};
static ColorRGBA blue = {.r = 0, .g = 0, .b = 255, .a = 255};
static ColorRGBA white = {.r = 255, .g = 255, .b = 255, .a = 255};
static ColorRGBA black = {.r = 0, .g = 0, .b = 0, .a = 255};

static ColorRGBA color_f32_to_u8(ColorRGBA_f32 c) {
  ColorRGBA res = {0};
  res.r = (u8)(f32_round_to_i32(255.0f * c.r) % 256);
  res.g = (u8)(f32_round_to_i32(255.0f * c.g) % 256);
  res.b = (u8)(f32_round_to_i32(255.0f * c.b) % 256);
  res.a = (u8)(f32_round_to_i32(255.0f * c.a) % 256);
  return res;
}

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
  runtime_assert(x >= 0 && x < (i32)state->window_width && y >= 0 && y < (i32)state->window_height);
  if (x >= 0 && x < (i32)state->window_width && y >= 0 &&
      y < (i32)state->window_height) {
    u32 i = ((u32)y * state->window_width + (u32)x);
    state->frame_buffer[i] = color;
  }
}

static void draw_line(RenderState *state, Vec2 a, Vec2 b, ColorRGBA color) {
  bool32 steep = f32_abs(a.x - b.x) < f32_abs(a.y - b.y);
  if (steep) {
    swap(f32, a.x, a.y);
    swap(f32, b.x, b.y);
  }
  if (a.x > b.x) {
    swap(f32, a.x, b.x);
    swap(f32, a.y, b.y);
  }
  for (i32 x = f32_round_to_i32(a.x); x < f32_round_to_i32(b.x); x++) {
    f32 t = (((f32)x - a.x)) / ((b.x - a.x));
    i32 y = f32_round_to_i32(a.y + (b.y - a.y) * t);
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
  m.m00 = b.x - a.x;
  m.m01 = c.x - a.x;
  m.m10 = b.y - a.y;
  m.m11 = c.y - a.y;
  return 0.5f * mat2_determinant(m);
}

static void draw_triangle(RenderState *state, Vec3 a, Vec3 b, Vec3 c,
                          ColorRGBA color) {
  i32 min_x, min_y, max_x, max_y;
  min_x = f32_round_to_i32(min(min(a.x, b.x), c.x));
  min_y = f32_round_to_i32(min(min(a.y, b.y), c.y));
  max_x = f32_round_to_i32(max(max(a.x, b.x), c.x));
  max_y = f32_round_to_i32(max(max(a.y, b.y), c.y));

  f32 total_area = signed_triangle_area(a, b, c);
  for (i32 y = max(0, min_y); y <= min((i32)state->window_height - 1, max_y);
       y++) {
    for (i32 x = max(0, min_x); x <= min((i32)state->window_width - 1, max_x);
         x++) {
      Vec3 p = {
          .x = (f32)x,
          .y = (f32)y,
          .z = 0.0f,
      };
      f32 alpha = signed_triangle_area(p, b, c) / total_area;
      f32 beta = signed_triangle_area(p, c, a) / total_area;
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

static Mat4 model_to_world_mat4(Object obj) {
  Mat4 scale = {0};
  scale.m00 = obj.scale;
  scale.m11 = obj.scale;
  scale.m22 = obj.scale;
  scale.m33 = 1.0f;

  Mat4 translate = mat4_identity;
  translate.m03 = obj.position.x;
  translate.m13 = obj.position.y;
  translate.m23 = obj.position.z;

  Mat4 rotate = quat_to_rotation_mat4(obj.rotation);

  return mat4_mult(translate, mat4_mult(rotate, scale));
}

static Mat4 world_to_view_mat4(Camera cam) {
  Mat4 translate = mat4_identity;
  translate.m03 = -cam.position.x;
  translate.m13 = -cam.position.y;
  translate.m23 = -cam.position.z;

  Mat4 rotate = quat_to_rotation_mat4(quat_conjugate(cam.rotation));

  // world coordinates: +x=right, +y=forwards, +z=up
  // view coordinates: +x=right, +y=up, +z=backwards
  Mat4 basis_change = {0};
  basis_change.m00 = 1.0f;
  basis_change.m12 = 1.0f;
  basis_change.m21 = -1.0f;
  basis_change.m33 = 1.0f;

  return mat4_mult(basis_change, mat4_mult(rotate, translate));
}

static Mat4 view_to_clip_mat4(Camera cam) {
  f32 top = cam.near_clip * f32_tan(cam.fov_y_radians / 2.0f);
  f32 right = top * cam.aspect_ratio;
  Mat4 projection = {0};
  projection.m00 = cam.near_clip / right;
  projection.m11 = cam.near_clip / top;
  projection.m22 =
      (cam.far_clip + cam.near_clip) / (cam.far_clip - cam.near_clip);
  projection.m23 =
      (2.0f * cam.far_clip * cam.near_clip) / (cam.far_clip - cam.near_clip);
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

typedef struct {
  Vec3 fragment_view_pos;
  Vec3 fragment_view_norm;
  Vec3 light_view_pos;
  u32 face_number;
} FragmentShaderData;

static ColorRGBA phong_shade(FragmentShaderData data) {
  Vec3 fragment_to_light =
      vec3_norm(vec3_sub(data.light_view_pos, data.fragment_view_pos));
  Vec3 fragment_to_camera = vec3_scale(vec3_norm(data.fragment_view_pos), -1);

  Vec3 reflection = vec3_sub(
      vec3_scale(data.fragment_view_norm,
                 2.0f * vec3_dot(data.fragment_view_norm, fragment_to_light)),
      fragment_to_light);

  f32 ambient = 0.05f;
  f32 diffuse = max(0.0f, vec3_dot(data.fragment_view_norm, fragment_to_light));
  f32 specular =
      f32_pow(max(0.0f, vec3_dot(reflection, fragment_to_camera)), 4);

  ColorRGBA_f32 res = {
      .r = min(1.0f, ambient + diffuse + specular),
      .g = min(1.0f, ambient + diffuse + specular),
      .b = min(1.0f, ambient + diffuse + specular),
      .a = 1.0f,
  };

  return color_f32_to_u8(res);
}

static void render_object_new(RenderState *state, Object obj) {
  Arena *scratch = get_scratch(0);
  rng_seed(0);

  Mat4 model_to_world = model_to_world_mat4(obj);
  Mat4 world_to_view = world_to_view_mat4(state->camera);
  Mat4 view_to_clip = view_to_clip_mat4(state->camera);

  Mat4 model_to_view = mat4_mult(world_to_view, model_to_world);
  Mat4 model_to_clip = mat4_mult(view_to_clip, model_to_view);

  Vec4 light_pos_world = (Vec4){.x = 10.0f, .y = 10.0f, .z = 10.0f, .w = 1.0f};
  Vec4 light_pos_view = mat4_vec4_mult(world_to_view, light_pos_world);

  u32 normal_count = obj.model.vertex_normals_count;
  Vec4 *mesh_normals = obj.model.vertex_normals;
  Vec4 *view_normals = arena_push_array(scratch, Vec4, normal_count);
  for (u32 i = 0; i < normal_count; i++) {
    view_normals[i] = mat4_vec4_mult(model_to_view, mesh_normals[i]);
  }

  u32 vertex_count = obj.model.mesh_vertex_count;
  Vec4 *mesh_vertices = obj.model.mesh_vertices;

  Vec4 *view_vertices = arena_push_array(scratch, Vec4, vertex_count);
  for (u32 i = 0; i < vertex_count; i++) {
    view_vertices[i] = mat4_vec4_mult(model_to_view, mesh_vertices[i]);
  }

  Vec4 *clip_vertices = arena_push_array(scratch, Vec4, vertex_count);
  for (u32 i = 0; i < vertex_count; i++) {
    clip_vertices[i] = mat4_vec4_mult(model_to_clip, mesh_vertices[i]);
  }

  Vec4 *ndc_vertices = arena_push_array(scratch, Vec4, vertex_count);
  for (u32 i = 0; i < vertex_count; i++) {
    if (clip_vertices[i].w != 0.0f) {
      ndc_vertices[i] = vec4_scale_down(clip_vertices[i], clip_vertices[i].w);
    }
  }

  bool32 *clipped = arena_push_array(scratch, bool32, vertex_count);
  for (u32 i = 0; i < vertex_count; i++) {
    f32 x = clip_vertices[i].x;
    f32 y = clip_vertices[i].y;
    f32 z = clip_vertices[i].z;
    f32 w = clip_vertices[i].w;
    clipped[i] = false;
    clipped[i] = !(x > -w && x < w && y > -w && y < w && z > -w && z < w);
  }

  for (u32 faces_index = 0; faces_index < obj.model.face_count; faces_index++) {
    Face face = obj.model.faces[faces_index];
    Vec3 screen_tri[3];

    if (clipped[face.vertex_indices[0]] || clipped[face.vertex_indices[1]] ||
        clipped[face.vertex_indices[2]]) {
      continue;
    }

    screen_tri[0] = ndc_to_screen(state, ndc_vertices[face.vertex_indices[0]]);
    screen_tri[1] = ndc_to_screen(state, ndc_vertices[face.vertex_indices[1]]);
    screen_tri[2] = ndc_to_screen(state, ndc_vertices[face.vertex_indices[2]]);

    // Rasterize
    i32 min_x, min_y, max_x, max_y;
    Vec3 a = screen_tri[0];
    Vec3 b = screen_tri[1];
    Vec3 c = screen_tri[2];
    min_x = f32_round_to_i32(min(min(a.x, b.x), c.x));
    min_y = f32_round_to_i32(min(min(a.y, b.y), c.y));
    max_x = f32_round_to_i32(max(max(a.x, b.x), c.x));
    max_y = f32_round_to_i32(max(max(a.y, b.y), c.y));

    f32 total_area = signed_triangle_area(a, b, c);
    for (i32 y = max(0, min_y); y <= min((i32)state->window_height - 1, max_y);
         y++) {
      for (i32 x = max(0, min_x); x <= min((i32)state->window_width - 1, max_x);
           x++) {
        Vec3 p = {
            .x = (f32)x,
            .y = (f32)y,
            .z = 0.0f,
        };
        f32 alpha = signed_triangle_area(p, b, c) / total_area;
        f32 beta = signed_triangle_area(p, c, a) / total_area;
        f32 gamma = signed_triangle_area(p, a, b) / total_area;
        if (!(alpha < 0.0f || beta < 0.0f || gamma < 0.0f)) {
          p.z = alpha * a.z + beta * b.z + gamma * c.z;
          u32 depth_index = (u32)y * state->window_width + (u32)x;
          runtime_assert(depth_index < state->depth_buffer_len);
          if (p.z > state->depth_buffer[depth_index]) {
            state->depth_buffer[depth_index] = p.z;

            // Interpolate fragment view pos and fragment normal
            // TODO: interpolate using view space barycentric coordinates
            Vec4 fragment_view_pos = vec4_lhat;
            fragment_view_pos.x =
                alpha * view_vertices[face.vertex_indices[0]].x +
                beta * view_vertices[face.vertex_indices[1]].x +
                gamma * view_vertices[face.vertex_indices[2]].x;
            fragment_view_pos.y =
                alpha * view_vertices[face.vertex_indices[0]].y +
                beta * view_vertices[face.vertex_indices[1]].y +
                gamma * view_vertices[face.vertex_indices[2]].y;
            fragment_view_pos.z =
                alpha * view_vertices[face.vertex_indices[0]].z +
                beta * view_vertices[face.vertex_indices[1]].z +
                gamma * view_vertices[face.vertex_indices[2]].z;

            Vec3 fragment_normal = vec3_zero;
            fragment_normal.x = alpha * view_normals[face.normal_indices[0]].x +
                                beta * view_normals[face.normal_indices[1]].x +
                                gamma * view_normals[face.normal_indices[2]].x;
            fragment_normal.y = alpha * view_normals[face.normal_indices[0]].y +
                                beta * view_normals[face.normal_indices[1]].y +
                                gamma * view_normals[face.normal_indices[2]].y;
            fragment_normal.z = alpha * view_normals[face.normal_indices[0]].z +
                                beta * view_normals[face.normal_indices[1]].z +
                                gamma * view_normals[face.normal_indices[2]].z;
            fragment_normal = vec3_norm(fragment_normal);

            FragmentShaderData data = {
                .fragment_view_norm = fragment_normal,
                .fragment_view_pos = vec3_from_vec4(fragment_view_pos),
                .face_number = faces_index,
                .light_view_pos = vec3_from_vec4(light_pos_view),
            };
            // vec3_from_vec4(fragment_view_pos),
            // vec3_from_vec4(light_pos_view), fragment_normal
            ColorRGBA color = phong_shade(data);

            draw_pixel(state, x, y, color);
          }
        }
      }
    }
  }
  free_scratch(scratch);
}

static void rasterize_triangle(RenderState *state, Vec4 A, Vec4 B, Vec4 C) {
  ColorRGBA color = random_color();

  if (A.x < A.w && A.x > -A.w && A.y < A.w && A.y > -A.w && A.z < A.w &&
      A.z > -A.w && B.x < B.w && B.x > -B.w && B.y < B.w && B.y > -B.w &&
      B.z < B.w && B.z > -B.w && C.x < C.w && C.x > -C.w && C.y < C.w &&
      C.y > -C.w && C.z < C.w && C.z > -C.w) {

    // Clip -> NDC
    A = vec4_scale_down(A, A.w);
    B = vec4_scale_down(B, B.w);
    C = vec4_scale_down(C, C.w);

    // NDC -> screen
    Vec3 a = ndc_to_screen(state, A);
    Vec3 b = ndc_to_screen(state, B);
    Vec3 c = ndc_to_screen(state, C);

    draw_triangle(state, a, b, c, color);
  }
}

static void render_2d_text(RenderState *state, Vec2 offset, Str8 str) {
  Arena *scratch = get_scratch(0);
  // easy font docs say each character will take ~270 bytes. We'll do 300 to be
  // safe.
  EasyFontVertex *vb =
      arena_push(scratch, 300 * str.len, align_of(EasyFontVertex));
  stb_easy_font_spacing(-0.5f);
  i32 num_quads = stb_easy_font_print(offset.x, offset.y, cstr(scratch, str),
                                      NULL, vb, 300 * (int)str.len);
  const f32 scale = 2.0f;
  for (i32 i = 0; i < num_quads * 4; i += 4) {
    Vec3 a = {
        .x = vb[i].x * scale,
        .y = vb[i].y * scale,
        .z = 100.0f,
    };
    Vec3 b = {
        .x = vb[i + 1].x * scale,
        .y = vb[i + 1].y * scale,
        .z = 100.0f,
    };
    Vec3 c = {
        .x = vb[i + 2].x * scale,
        .y = vb[i + 2].y * scale,
        .z = 100.0f,
    };
    Vec3 d = {
        .x = vb[i + 3].x * scale,
        .y = vb[i + 3].y * scale,
        .z = 100.0f,
    };
    draw_triangle(state, a, b, c, white);
    draw_triangle(state, c, d, a, white);
  }
  free_scratch(scratch);
}

static void update_camera(RenderState *state, f32 dt) {
  state->camera.aspect_ratio =
      (f32)state->window_width / (f32)state->window_height;
  f32 movement_speed = 10.0f; // meters per second
  Vec4 view_delta = {
      .x = state->controls.left_right,
      .y = state->controls.up_down,
      .z = -state->controls.forward_backward,
      .w = 0.0f,
  };

  if (!vec4_is_equal(view_delta, vec4_zero)) {
    view_delta = vec4_scale(vec4_norm(view_delta), movement_speed * dt);
  }

  Mat4 view_to_world_rotate = quat_to_rotation_mat4(state->camera.rotation);
  Mat4 basis_change = {0};
  basis_change.m00 = 1.0f;
  basis_change.m12 = -1.0f;
  basis_change.m21 = 1.0f;
  basis_change.m33 = 1.0f;
  Mat4 view_to_world = mat4_mult(view_to_world_rotate, basis_change);
  Vec4 world_delta = mat4_vec4_mult(view_to_world, view_delta);
  state->camera.position = vec4_add(state->camera.position, world_delta);

  f32 roll_input = state->controls.look_roll;
  f32 pitch_input = state->controls.look_pitch;
  f32 yaw_input = state->controls.look_yaw;
  f32 roll_speed = 2.0f * f32_pi;  // radians per second
  f32 pitch_speed = 2.0f * f32_pi; // radians per second
  f32 yaw_speed = 2.0f * f32_pi;   // radians per second

  f32 roll_angle = roll_input * roll_speed * dt;
  f32 pitch_angle = pitch_input * pitch_speed * dt;
  f32 yaw_angle = yaw_input * yaw_speed * dt;

  // Roll in local camera space (not world space)
  Quat roll_delta = quat_from_axis_angle(roll_angle, vec3_jhat);
  Quat pitch_delta = quat_from_axis_angle(pitch_angle, vec3_ihat);
  Quat yaw_delta = quat_from_axis_angle(yaw_angle, vec3_khat);

  Quat delta = quat_mult(yaw_delta, quat_mult(pitch_delta, roll_delta));

  state->camera.rotation = quat_norm(quat_mult(state->camera.rotation, delta));
}

static void update_and_render(RenderState *state, f32 dt) {
  runtime_assert(state->window_width * state->window_height <=
                 state->frame_buffer_len);
  update_camera(state, dt);

  u64 start = os_get_usec();
  clear_frame_buffer(state, black);
  reset_depth_buffer(state);
  // draw_object(state, *state->obj);
  render_object_new(state, state->obj[0]);
  u64 end = os_get_usec();

  f32 ms = (f32)(end - start) / 1000.0f;

  char debug_text[1000] = {0};
  snprintf(
      debug_text, 1000,
      "dt %f s\nCamera pos (%f, %f, %f)\nCamera rot (%f, %f, %f, %f)\n%f ms",
      (f64)dt, (f64)state->camera.position.x, (f64)state->camera.position.y,
      (f64)state->camera.position.z, (f64)state->camera.rotation.w,
      (f64)state->camera.rotation.x, (f64)state->camera.rotation.y,
      (f64)state->camera.rotation.z, (f64)ms);
  render_2d_text(state, vec2_zero, s(debug_text));
}
