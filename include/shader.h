#ifndef SHADER_H_
#define SHADER_H_

#include "mathc.h"

unsigned int createShader(const char* vertexFile, const char* fragmentFile);
void setInt(unsigned int shaderID, const char* name, int value);
void setFloat(unsigned int shaderID, const char* name, float value);
void setVec3f(unsigned int shaderID, const char* name, float x, float y, float z);
void setVec3fv(unsigned int shaderID, const char* name, mfloat_t* v);
void setMat4fv(unsigned int shaderID, const char* name, mfloat_t* m);
void detachShader();
void destroyShader(unsigned int shaderID);

#endif
