#include "shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "file.h"

unsigned int createShader(const char *vertexFile, const char *fragmentFile) {
    GLint success = 0;
    GLint logSize = 0;

    // Create a shader object and compile it during runtime
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char *vertexShaderSource = readFile(vertexFile);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logSize);
        GLchar infoLog[logSize];
        glGetShaderInfoLog(vertexShader, logSize, &logSize, infoLog);
        glDeleteShader(vertexShader);
        fprintf(stderr, "Vertex Shader: %s\n", infoLog);
    }

    // Perform the same steps for the fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fragmentShaderSource = readFile(fragmentFile);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logSize);
        GLchar infoLog[logSize];
        glGetShaderInfoLog(fragmentShader, logSize, &logSize, infoLog);
        glDeleteShader(fragmentShader);
        fprintf(stderr, "Fragment Shader: %s\n", infoLog);
    }

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

void detachShader() {
    glUseProgram(0);
}

void destroyShader(unsigned int shaderID) {
    glDeleteProgram(shaderID);
}
