#ifndef MODEL_H_
#define MODEL_H_

#include <assimp/cimport.h>
#include <assimp/scene.h>

#include "mesh.h"
#include "util.h"

typedef struct {
    DynamicArray* meshes;
    DynamicArray* textures_loaded;
    char* directory;
    bool gammaCorrection;
} Model;

Model* createModel(const char* path);
void drawMeshesInModel(Model* model, unsigned int shaderID);
void loadModel(Model* model, const char* path);
void processNode(Model* model, struct aiNode* node, const struct aiScene* scene);
Mesh* processMesh(Model* model, struct aiMesh* mesh, const struct aiScene* scene);
DynamicArray* loadMaterialTextures(Model* model, struct aiMaterial* mat, enum aiTextureType type, char* typeName);

#endif
