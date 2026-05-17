#pragma once

#include "core/core.h"
#include "core/arena.h"

typedef Vec4 GeometricVertex;
typedef Vec3 TextureVertex;
typedef Vec3 VertexNormal;
typedef struct {
    u32 vertex_indices[3];
    u32 texture_indices[3];
    u32 normal_indices[3];
} Face;

typedef struct {
    GeometricVertex *vertices;
    u32 vertex_count;
    TextureVertex *texture_coords;
    u32 texture_coord_count;
    VertexNormal *vertex_norms;
    u32 vertex_norm_count;
    Face *faces;
    u32 face_count;
} Mesh;

Mesh *meshFromObj(Arena *arena, char *file_path);
