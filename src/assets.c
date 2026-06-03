#include "assets.h"
#include "core.h"
#include "os.h"
#include "render.h"

#include <stdio.h>
#include <stdlib.h>

static Model *model_from_obj(Arena *arena, Str8 file_path) {
    Arena scratch = {0};
    Model *res = arena_push_struct(arena, Model);

    Str8 contents = os_read_entire_file(&scratch, file_path);
    // println(contents);
    Str8Node *tokens = str8_split(&scratch, contents, ws_delims);

    // Pre-pass to get size of arrays needed and trim
    Str8Node *curr = tokens;
    while (curr) {
        curr->str = str8_trim(curr->str);
        if (str8_equal(curr->str, s("f"))) {
            res->face_count++;
        } else if (str8_equal(curr->str, s("vt"))) {
            res->texture_vertex_count++;
        } else if (str8_equal(curr->str, s("vn"))) {
            res->vertex_normals_count++;
        } else if (str8_equal(curr->str, s("v"))) {
            res->mesh_vertex_count++;
        }
        curr = curr->next;
    }

    res->faces = arena_push_array(arena, Face, res->face_count);
    res->texture_vertices = arena_push_array(arena, Vec3, res->texture_vertex_count);
    res->vertex_normals = arena_push_array(arena, Vec3, res->vertex_normals_count);
    res->mesh_vertices = arena_push_array(arena, Vec4, res->mesh_vertex_count);

    u32 mesh_vertex_index = 0;
    u32 texture_vertex_index = 0;
    u32 vertex_normal_index = 0;
    u32 face_index = 0;
    curr = tokens;
    while (curr) {
        if (str8_equal(curr->str, s("f"))) {
            curr = curr->next;
            // TODO: handle more than 3 vertices (quads)
            // TODO: handle other formats (e.g. 1//2, 3/1, 1 2 3, etc.)
            res->faces[face_index].flags = FACE_NORMALS | FACE_POSITIONS | FACE_TEXCOORDS;
            for (u32 vert = 0; vert < 3; vert++) {
                Str8Node *nums = str8_split(&scratch, curr->str, s("/"));
                res->faces[face_index].vertex_indices[vert] = (u32)str8_parse_i32(nums->str)-1;
                nums=nums->next;
                res->faces[face_index].texture_indices[vert] = (u32)str8_parse_i32(nums->str)-1;
                nums=nums->next;
                res->faces[face_index].normal_indices[vert] = (u32)str8_parse_i32(nums->str)-1;
                curr = curr->next;
            }

            // Face f = res->faces[face_index];
            // printf("Face[%u]:\t", face_index);
            // printf("%u/%u/%u\t", f.vertex_indices[0], f.texture_indices[0], f.normal_indices[0]);
            // printf("%u/%u/%u\t", f.vertex_indices[1], f.texture_indices[1], f.normal_indices[1]);
            // printf("%u/%u/%u\n", f.vertex_indices[2], f.texture_indices[2], f.normal_indices[2]);

            face_index++;
        } else if (str8_equal(curr->str, s("vt"))) {
            curr = curr->next;
            res->texture_vertices[texture_vertex_index].x = str8_parse_f32(curr->str);
            curr = curr->next;
            res->texture_vertices[texture_vertex_index].y = str8_parse_f32(curr->str); // TODO: handle case of optional v
            curr = curr->next;
            res->texture_vertices[texture_vertex_index].z = str8_parse_f32(curr->str); // TODO: handle case of optional w
            texture_vertex_index++;
            curr = curr->next;
        } else if (str8_equal(curr->str, s("vn"))) {
            curr = curr->next;
            res->vertex_normals[vertex_normal_index].x = str8_parse_f32(curr->str);
            curr = curr->next;
            res->vertex_normals[vertex_normal_index].y = str8_parse_f32(curr->str);
            curr = curr->next;
            res->vertex_normals[vertex_normal_index].z = str8_parse_f32(curr->str);
            vertex_normal_index++;
            curr = curr->next;
        } else if (str8_equal(curr->str, s("v"))) {
            curr = curr->next;
            res->mesh_vertices[mesh_vertex_index].x = str8_parse_f32(curr->str);
            curr = curr->next;
            res->mesh_vertices[mesh_vertex_index].y = str8_parse_f32(curr->str);
            curr = curr->next;
            res->mesh_vertices[mesh_vertex_index].z = str8_parse_f32(curr->str);
            // curr = curr->next;
            res->mesh_vertices[mesh_vertex_index].w = 1.0f; // TODO: handle case of explicit w coordinate

            // Vec4 v = res->mesh_vertices[mesh_vertex_index];
            // printf("Vertex[%u]:\t", mesh_vertex_index);
            // printf("%f\t%f\t%f\n", v.x, v.y, v.z);

            mesh_vertex_index++;
            curr = curr->next;
        } else {
            curr = curr->next;
        }
    }

    arena_destroy(&scratch);
    return res;
}
