#ifndef CAMERA_H_
#define CAMERA_H_

#include <GL/glew.h>

#include "mathc.h"

#define YAW -90.0f
#define PITCH 0.0f
#define SPEED 2.5f
#define SENSITIVITY 0.1f
#define ZOOM 45.0f

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

typedef struct {
    mfloat_t yaw;
    mfloat_t pitch;
    mfloat_t movementSpeed;
    mfloat_t mouseSensitivity;
    mfloat_t zoom;
    mfloat_t position[VEC3_SIZE];
    mfloat_t forward[VEC3_SIZE];
    mfloat_t right[VEC3_SIZE];
    mfloat_t up[VEC3_SIZE];
    mfloat_t worldUp[VEC3_SIZE];
} Camera;

Camera* createCamera(mfloat_t* position);
void updateVectors(Camera* camera);
mfloat_t* getViewMatrix(Camera* camera, mfloat_t* viewMatrix);
void processKeyboard(Camera* camera, enum Camera_Movement direction, float deltaTime);
void processMouseMovement(Camera* camera, float xoffset, float yoffset, GLboolean constrainPitch);
void processMouseScroll(Camera* camera, float yoffset);

#endif
