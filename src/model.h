#pragma once

#include "core/core.h"
#include "core/arena.h"

typedef enum {
    FACE_POSITIONS = 1 << 0,
    FACE_TEXCOORDS = 1 << 1,
    FACE_NORMALS   = 1 << 2  
} FaceFlags;

typedef struct {
    u32 vertex_indices[3];
    u32 texture_indices[3];
    u32 normal_indices[3];
    FaceFlags flags;
} Face;

typedef struct {
    Vec4 *vertices;
    u32 vertex_count;
    Vec3 *texture_vertices;
    u32 texture_vertex_count;
    Vec3 *vertex_normals;
    u32 vertex_normals_count;

    Face *faces;
    u32 face_count;
} Mesh;

Mesh *mesh_from_obj(Arena *arena, char *file_path);
