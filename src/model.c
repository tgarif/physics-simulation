#include "model.h"

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assimp/cimport.h"
#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "mesh.h"
#include "util.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

char* custom_strdup(const char* str);
char* concatenatePath(const char* directory, const char* path);
unsigned int TextureFromFile(const char* path, const char* directory, bool gamma);

Model* createModel(const char* path) {
    Model* model = malloc(sizeof(Model));

    DynamicArray* meshes = malloc(sizeof(DynamicArray));
    initialize(meshes, 160, sizeof(Mesh));
    DynamicArray* textures_loaded = malloc(sizeof(DynamicArray));
    initialize(textures_loaded, 256, sizeof(Texture));

    model->meshes = meshes;
    model->textures_loaded = textures_loaded;

    loadModel(model, path);

    return model;
}

void drawMeshesInModel(Model* model, unsigned int shaderID) {
    Mesh* meshes = (Mesh*)model->meshes->array;

    for (size_t i = 0; i < model->meshes->size; ++i)
        drawMesh(&meshes[i], shaderID);
}

void loadModel(Model* model, const char* path) {
    const struct aiScene* scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        fprintf(stderr, "Error! Assimp: %s\n", aiGetErrorString());
        exit(EXIT_FAILURE);
    }

    const char* lastSlash = strrchr(path, '/');
    if (lastSlash != NULL) {
        size_t length = lastSlash - path;

        model->directory = (char*)malloc(length + 1);
        if (model->directory == NULL) {
            fprintf(stderr, "Failed to allocate memory for directory");
            exit(EXIT_FAILURE);
        }

        strncpy(model->directory, path, length);
        model->directory[length] = '\0';
    } else {
        model->directory = custom_strdup("");
        if (model->directory == NULL) {
            fprintf(stderr, "Failed to allocate memory for directory");
            exit(EXIT_FAILURE);
        }
    }

    processNode(model, scene->mRootNode, scene);
}

void processNode(Model* model, struct aiNode* node, const struct aiScene* scene) {
    for (size_t i = 0; i < node->mNumMeshes; i++) {
        struct aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        push(model->meshes, processMesh(model, mesh, scene));
    }

    for (size_t i = 0; i < node->mNumChildren; i++) {
        processNode(model, node->mChildren[i], scene);
    }
}

Mesh* processMesh(Model* model, struct aiMesh* mesh, const struct aiScene* scene) {
    DynamicArray* vertices = malloc(sizeof(DynamicArray));
    DynamicArray* indices = malloc(sizeof(DynamicArray));
    DynamicArray* textures = malloc(sizeof(DynamicArray));

    initialize(vertices, 1024, sizeof(Vertex));
    initialize(indices, 1024, sizeof(unsigned int));
    initialize(textures, 256, sizeof(Texture));

    for (size_t i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.position[0] = mesh->mVertices[i].x;
        vertex.position[1] = mesh->mVertices[i].y;
        vertex.position[2] = mesh->mVertices[i].z;

        if (mesh->mNormals != NULL) {
            vertex.normal[0] = mesh->mNormals[i].x;
            vertex.normal[1] = mesh->mNormals[i].y;
            vertex.normal[2] = mesh->mNormals[i].z;
        }

        if (mesh->mTextureCoords[0]) {
            vertex.texCoords[0] = mesh->mTextureCoords[0][i].x;
            vertex.texCoords[1] = mesh->mTextureCoords[0][i].y;

            if (mesh->mTangents != NULL) {
                vertex.tangent[0] = mesh->mTangents[i].x;
                vertex.tangent[1] = mesh->mTangents[i].y;
                vertex.tangent[2] = mesh->mTangents[i].z;
            } else {
                vertex.tangent[0] = 0.0f;
                vertex.tangent[1] = 0.0f;
                vertex.tangent[2] = 0.0f;
            }

            if (mesh->mBitangents != NULL) {
                vertex.bitangent[0] = mesh->mBitangents[i].x;
                vertex.bitangent[1] = mesh->mBitangents[i].y;
                vertex.bitangent[2] = mesh->mBitangents[i].z;
            } else {
                vertex.bitangent[0] = 0.0f;
                vertex.bitangent[1] = 0.0f;
                vertex.bitangent[2] = 0.0f;
            }
        } else {
            vertex.texCoords[0] = 0.0f;
            vertex.texCoords[1] = 0.0f;
        }

        push(vertices, &vertex);
    }

    for (size_t i = 0; i < mesh->mNumFaces; i++) {
        struct aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
            push(indices, &face.mIndices[j]);
    }

    struct aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // 1. diffuse maps
    DynamicArray* diffuseMaps = loadMaterialTextures(model, material, aiTextureType_DIFFUSE, "texture_diffuse");
    insert(textures, textures->size, diffuseMaps->array, diffuseMaps->size);

    // 2. specular maps
    DynamicArray* specularMaps = loadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");
    insert(textures, textures->size, specularMaps->array, specularMaps->size);

    // 3. normal maps
    DynamicArray* normalMaps = loadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");
    insert(textures, textures->size, normalMaps->array, normalMaps->size);

    // 4. height maps
    DynamicArray* heightMaps = loadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");
    insert(textures, textures->size, heightMaps->array, heightMaps->size);

    return createMesh(vertices, indices, textures);
}

DynamicArray* loadMaterialTextures(Model* model, struct aiMaterial* mat, enum aiTextureType type, char* typeName) {
    DynamicArray* textures = malloc(sizeof(DynamicArray));
    initialize(textures, 256, sizeof(Texture));

    Texture* texturesLoadedArr = (Texture*)model->textures_loaded->array;

    for (size_t i = 0; i < aiGetMaterialTextureCount(mat, type); i++) {
        struct aiString str;
        aiGetMaterialTexture(mat, type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);
        bool skip = false;
        for (size_t j = 0; j < model->textures_loaded->size; j++) {
            if (strcmp(texturesLoadedArr[j].path.data, str.data) == 0) {
                push(textures, &texturesLoadedArr[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture;
            texture.id = TextureFromFile(str.data, model->directory, true);
            texture.type = typeName;
            texture.path = str;
            push(textures, &texture);
            push(model->textures_loaded, &texture);
        }
    }
    return textures;
}

char* custom_strdup(const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char* copy = malloc(len);
    if (copy) strcpy(copy, str);
    return copy;
}

char* concatenatePath(const char* directory, const char* path) {
    size_t dirLen = strlen(directory);
    size_t pathLen = strlen(path);
    size_t totalLen = dirLen + pathLen + 2;  // For '/' and null terminator

    char* fullPath = malloc(totalLen);
    if (fullPath == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    snprintf(fullPath, totalLen, "%s/%s", directory, path);
    return fullPath;
}

unsigned int TextureFromFile(const char* path, const char* directory, bool gamma) {
    char* filename = concatenatePath(directory, path);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        fprintf(stderr, "Texture failed to load at path: %s\n", filename);
        stbi_image_free(data);
    }

    return textureID;
}
