#ifndef SHADER_H_
#define SHADER_H_

unsigned int createShader(const char* vertexFile, const char* fragmentFile);

void setInt(unsigned int shaderID, const char* name, int value);

void detachShader();

void destroyShader(unsigned int shaderID);

#endif
