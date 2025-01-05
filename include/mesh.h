#ifndef MESH_H_
#define MESH_H_

#include "assimp/types.h"
#include "mathc.h"
#include "util.h"

#define MAX_BONE_INFLUENCE 4

typedef struct {
    mfloat_t position[VEC3_SIZE];
    mfloat_t normal[VEC3_SIZE];
    mfloat_t texCoords[VEC2_SIZE];
    mfloat_t tangent[VEC3_SIZE];
    mfloat_t bitangent[VEC3_SIZE];
    int mBoneIds[MAX_BONE_INFLUENCE];
    float mWeights[MAX_BONE_INFLUENCE];
} Vertex;

typedef struct {
    unsigned int id;
    char* type;
    struct aiString path;
} Texture;

typedef struct {
    DynamicArray* vertices;
    DynamicArray* indices;
    DynamicArray* textures;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
} Mesh;

Mesh* createMesh(DynamicArray* vertices, DynamicArray* indices, DynamicArray* textures);
void setupMesh(Mesh* mesh);
void drawMesh(Mesh* mesh, unsigned int shaderID);

#endif
