#include "shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

unsigned int createShader(const char *vertexFile, const char *fragmentFile) {
    GLint success = 0;
    GLint logSize = 0;

    // Create a shader object and compile it during runtime
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    char *vertexShaderSource = readFile(vertexFile);
    glShaderSource(vertexShader, 1, (const char **)&vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logSize);
        GLchar infoLog[logSize];
        glGetShaderInfoLog(vertexShader, logSize, &logSize, infoLog);
        glDeleteShader(vertexShader);
        fprintf(stderr, "Vertex Shader: %s\n", infoLog);
    }

    free(vertexShaderSource);

    // Perform the same steps for the fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char *fragmentShaderSource = readFile(fragmentFile);
    glShaderSource(fragmentShader, 1, (const char **)&fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logSize);
        GLchar infoLog[logSize];
        glGetShaderInfoLog(fragmentShader, logSize, &logSize, infoLog);
        glDeleteShader(fragmentShader);
        fprintf(stderr, "Fragment Shader: %s\n", infoLog);
    }

    free(fragmentShaderSource);

    // Create a shader program and link the two shader steps together
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logSize);
        GLchar infoLog[logSize];
        glGetProgramInfoLog(shaderProgram, logSize, &logSize, infoLog);
        glDeleteProgram(shaderProgram);
        fprintf(stderr, "Shader Program: %s\n", infoLog);
    }

    // Make sure to cleanup the individual shaders after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void setInt(unsigned int shaderID, const char *name, int value) {
    glUniform1i(glGetUniformLocation(shaderID, name), value);
}

void setFloat(unsigned int shaderID, const char *name, float value) {
    glUniform1f(glGetUniformLocation(shaderID, name), value);
}

void setVec3f(unsigned int shaderID, const char *name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(shaderID, name), x, y, z);
}

void setVec3fv(unsigned int shaderID, const char *name, mfloat_t *v) {
    glUniform3fv(glGetUniformLocation(shaderID, name), 1, v);
}

void setMat4fv(unsigned int shaderID, const char *name, mfloat_t *m) {
    glUniformMatrix4fv(glGetUniformLocation(shaderID, name), 1, GL_FALSE, m);
}

void detachShader() {
    glUseProgram(0);
}

void destroyShader(unsigned int shaderID) {
    glDeleteProgram(shaderID);
}
