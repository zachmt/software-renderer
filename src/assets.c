#include "assets.h"
#include "core.h"
#include <stdio.h>
#include <stdlib.h>

static Vec4 parse_vertex(char *input) {
    char *num_start = 0;
    u32 index = 0;
    Vec4 res = {0};
    res.w = 1;
    while(*input != '\n' && *input != '\0' && index <= 3) {
        if ((*input >= '0' && *input <= '9') || *input == '-') {
            num_start = input;
            while((*input >= '0' && *input <= '9') || *input == '.' || *input == '-') {
                input++;
            }
            *input = '\0';
            res.v[index] = (f32)atof(num_start);
            index++;
        }
        input++;
    }
    return res;
}

static Face parse_face(char *input) {
    char *num_start = 0;
    Face face = {0};
    face.flags = FACE_POSITIONS | FACE_TEXCOORDS | FACE_NORMALS;
    u32 index = 0;
    u32 type = 0;
    while(*input != '\n' && *input != '\0' && index <= 2) {
        if ((*input >= '0' && *input <= '9') || *input == '-') {
            num_start = input;
            while((*input >= '0' && *input <= '9') || *input == '-') {
                input++;
            }
            *input = '\0';
            switch (type) {
                // TODO: support negative indices
                case 0: {
                            face.vertex_indices[index] = (u32)atoi(num_start) - 1;
                        } break;
                case 1: {
                            face.texture_indices[index] = (u32)atoi(num_start) - 1;
                        } break;
                case 2: {
                            face.normal_indices[index] = (u32)atoi(num_start) - 1;
                            index++;
                        } break;
                default:{
                        } break;
            }
            type++;
            type %= 3;
        }
        input++;
    }
    return face;
}

static Model *model_from_obj(Arena *arena, char *file_path) {
    FILE *obj_file = fopen(file_path, "r");
    if (obj_file == 0) {
        return 0;
    }

    Model *model = (Model *)arena_push(arena, sizeof(Model), align_of(Model));
    model->mesh_vertex_count = 0;
    model->texture_vertex_count = 0;
    model->vertex_normals_count = 0;
    model->face_count = 0;

    char line_buf[1024];
    while (fgets(line_buf, 1024, obj_file)) {
        if (line_buf[0] == 'v' && line_buf[1] == ' ') {
            model->mesh_vertex_count++;
        } else if (line_buf[0] == 'v' && line_buf[1] == 't') {
            model->texture_vertex_count++;
        } else if (line_buf[0] == 'v' && line_buf[1] == 'n') {
            model->vertex_normals_count++;
        } else if(line_buf[0] == 'f') {
            model->face_count++;
        }
    }
    rewind(obj_file);

    model->mesh_vertices = (Vec4 *)arena_push(arena, sizeof(Vec4) * model->mesh_vertex_count, align_of(Vec4));
    model->texture_vertices = (Vec3 *)arena_push(arena, sizeof(Vec3) * model->texture_vertex_count, align_of(Vec3));
    model->vertex_normals = (Vec3 *)arena_push(arena, sizeof(Vec3) * model->vertex_normals_count, align_of(Vec3));
    model->faces = (Face *)arena_push(arena, sizeof(Face) * model->face_count, align_of(Face));

    Vec4 *current_vertex = model->mesh_vertices;
    Face *current_face = model->faces;
    while (fgets(line_buf, 1024, obj_file)) {
        if (line_buf[0] == 'v' && line_buf[1] == ' ') {
            *current_vertex = parse_vertex(line_buf);
            current_vertex++;
        } else if (line_buf[0] == 'v' && line_buf[1] == 't') {
            // TODO
        } else if (line_buf[0] == 'v' && line_buf[1] == 'n') {
            // TODO
        } else if(line_buf[0] == 'f') {
            *current_face = parse_face(line_buf);
            current_face++;
        }
    }

    fclose(obj_file);
    return model;
}
