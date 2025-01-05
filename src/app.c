#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "mathc.h"
#include "model.h"
#include "shader.h"
#include "stb_image.h"

#define factor 80
#define WINDOW_WIDTH (factor * 12)
#define WINDOW_HEIGHT (factor * 9)

Camera* camera;
bool firstMouse = true;
float lastX = (float)WINDOW_WIDTH / 2.0, lastY = (float)WINDOW_HEIGHT / 2.0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Functions prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    unsigned int modelShader = createShader("shaders/model_loading_vertex.glsl", "shaders/model_loading_fragment.glsl");

    Model* ourModel = createModel("models/backpack.obj");

    camera = createCamera((mfloat_t[]){0.0f, 0.0f, 3.0f});

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(modelShader);

        // Projection matrix
        mfloat_t projection[MAT4_SIZE];
        mat4_identity(projection);
        mat4_perspective(projection, MRADIANS(camera->zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        setMat4fv(modelShader, "projection", projection);

        // View matrix
        mfloat_t view[MAT4_SIZE];
        getViewMatrix(camera, view);
        setMat4fv(modelShader, "view", view);

        mfloat_t model[MAT4_SIZE];
        mat4_identity(model);
        mfloat_t translated[MAT4_SIZE];
        mfloat_t scaled[MAT4_SIZE];
        mat4_translate(translated, model, (mfloat_t[]){0.0f, 0.0f, 0.0f});
        mat4_scale(scaled, model, (mfloat_t[]){1.0f, 1.0f, 1.0f});
        mat4_multiply(model, translated, scaled);
        setMat4fv(modelShader, "model", model);
        drawMeshesInModel(ourModel, modelShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free(camera);

    glfwTerminate();
    return EXIT_SUCCESS;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        processKeyboard(camera, FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        processKeyboard(camera, BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        processKeyboard(camera, LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        processKeyboard(camera, RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    processMouseMovement(camera, xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    processMouseScroll(camera, (float)yoffset);
}
