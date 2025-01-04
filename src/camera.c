#include "camera.h"

#include <stdlib.h>

#include "mathc.h"

Camera* createCamera(mfloat_t* position) {
    Camera* camera = malloc(sizeof(Camera));
    vec3_assign(camera->forward, (mfloat_t[]){0.0f, 0.0f, -1.0f});
    camera->movementSpeed = SPEED;
    camera->mouseSensitivity = SENSITIVITY;
    camera->zoom = ZOOM;
    vec3_assign(camera->position, position);
    vec3_assign(camera->worldUp, (mfloat_t[]){0.0f, 1.0f, 0.0f});
    camera->yaw = YAW;
    camera->pitch = PITCH;
    return camera;
}

void updateVectors(Camera* camera) {
    vec3(camera->forward,
        MCOS(MRADIANS(camera->yaw)) * MCOS(MRADIANS(camera->pitch)),
        MSIN(MRADIANS(camera->pitch)),
        MSIN(MRADIANS(camera->yaw)) * MCOS(MRADIANS(camera->pitch)));
    vec3_normalize(camera->forward, camera->forward);
    vec3_cross(camera->right, camera->forward, camera->worldUp);
    vec3_normalize(camera->right, camera->right);
    vec3_cross(camera->up, camera->right, camera->forward);
    vec3_normalize(camera->up, camera->up);
}

mfloat_t* getViewMatrix(Camera* camera, mfloat_t* viewMatrix) {
    // updateVectors()
    mfloat_t target[VEC3_SIZE];
    vec3_add(target, camera->position, camera->forward);
    mat4_look_at(viewMatrix, camera->position, target, camera->up);
    return viewMatrix;
}

void processKeyboard(Camera* camera, enum Camera_Movement direction, float deltaTime) {
    float velocity = camera->movementSpeed * deltaTime;
    mfloat_t multiply[VEC3_SIZE];
    if (direction == FORWARD)
        vec3_add(camera->position, camera->position, vec3_multiply_f(multiply, camera->forward, velocity));
    if (direction == BACKWARD)
        vec3_subtract(camera->position, camera->position, vec3_multiply_f(multiply, camera->forward, velocity));
    if (direction == LEFT)
        vec3_subtract(camera->position, camera->position, vec3_multiply_f(multiply, camera->right, velocity));
    if (direction == RIGHT)
        vec3_add(camera->position, camera->position, vec3_multiply_f(multiply, camera->right, velocity));
}

void processMouseMovement(Camera* camera, float xoffset, float yoffset, GLboolean constrainPitch) {
    xoffset *= camera->mouseSensitivity;
    yoffset *= camera->mouseSensitivity;

    camera->yaw += xoffset;
    camera->pitch += yoffset;

    if (constrainPitch) {
        if (camera->pitch > 89.0f)
            camera->pitch = 89.0f;
        if (camera->pitch < -89.0f)
            camera->pitch = -89.0f;
    }

    updateVectors(camera);
}

void processMouseScroll(Camera* camera, float yoffset) {
    camera->zoom -= (float)yoffset;
    if (camera->zoom < 1.0f)
        camera->zoom = 1.0f;
    if (camera->zoom > 45.0f)
        camera->zoom = 45.0f;
}
