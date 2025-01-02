#ifndef SHADER_H_
#define SHADER_H_

unsigned int createShader(const char* vertexFile, const char* fragmentFile);

void detachShader();

void destroyShader(unsigned int shaderID);

#endif
