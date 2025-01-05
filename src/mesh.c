#include "mesh.h"

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Mesh* createMesh(DynamicArray* vertices, DynamicArray* indices, DynamicArray* textures) {
    Mesh* mesh = malloc(sizeof(Mesh));
    mesh->vertices = vertices;
    mesh->indices = indices;
    mesh->textures = textures;

    setupMesh(mesh);

    return mesh;
}

void setupMesh(Mesh* mesh) {
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);

    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertices->size * sizeof(Vertex), &mesh->vertices->array[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices->size * sizeof(unsigned int), &mesh->indices->array[0], GL_STATIC_DRAW);

    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, mBoneIds));
    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, mWeights));
    glBindVertexArray(0);
}

void drawMesh(Mesh* mesh, unsigned int shaderID) {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    Texture* textures = (Texture*)mesh->textures->array;

    for (unsigned int i = 0; i < mesh->textures->size; i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        char number[12];
        char* name = textures[i].type;
        if (strcmp(name, "texture_diffuse") == 0)
            snprintf(number, sizeof(number), "%u", diffuseNr++);
        if (strcmp(name, "texture_specular") == 0)
            snprintf(number, sizeof(number), "%u", specularNr++);
        if (strcmp(name, "texture_normal") == 0)
            snprintf(number, sizeof(number), "%u", normalNr++);
        if (strcmp(name, "texture_height") == 0)
            snprintf(number, sizeof(number), "%u", heightNr++);

        char uniformName[64];
        snprintf(uniformName, sizeof(uniformName), "%s%s", name, number);

        glUniform1i(glGetUniformLocation(shaderID, uniformName), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)mesh->indices->size, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}
