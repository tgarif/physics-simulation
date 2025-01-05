#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "mathc.h"
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define factor 80
#define WINDOW_WIDTH (factor * 12)
#define WINDOW_HEIGHT (factor * 9)
#define TOTAL_CUBE 10
#define insertCubePos(idx, x, y, z) (vec3(cubePositions[idx], x, y, z))

Camera* camera;
bool firstMouse = true;
float lastX = (float)WINDOW_WIDTH / 2.0, lastY = (float)WINDOW_HEIGHT / 2.0;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

mfloat_t lightPos[VEC3_SIZE] = {1.2f, 1.0f, 2.0f};

// Functions prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);

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

    glEnable(GL_DEPTH_TEST);

    unsigned int lightingShader = createShader("shaders/vertex.glsl", "shaders/fragment.glsl");
    unsigned int lightCubeShader = createShader("shaders/lighting_vertex.glsl", "shaders/lighting_fragment.glsl");

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f  // Hello
    };

    mfloat_t cubePositions[TOTAL_CUBE][VEC3_SIZE];
    insertCubePos(0, 0.0f, 0.0f, 0.0f);
    insertCubePos(1, 2.0f, 5.0f, -15.0f);
    insertCubePos(2, -1.5f, -2.2f, -2.5f);
    insertCubePos(3, -3.8f, -2.0f, -12.3f);
    insertCubePos(4, 2.4f, -0.4f, -3.5f);
    insertCubePos(5, -1.7f, 3.0f, -7.5f);
    insertCubePos(6, 1.3f, -2.0f, -2.5f);
    insertCubePos(7, 1.5f, 2.0f, -2.5f);
    insertCubePos(8, 1.5f, 0.2f, -1.5f);
    insertCubePos(9, -1.3f, 1.0f, -1.5f);

    unsigned int VBO, cubeVAO, lightCubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int diffuseMap = loadTexture("textures/container2.png");
    unsigned int specularMap = loadTexture("textures/container2_specular.png");

    camera = createCamera((mfloat_t[]){0.0f, 0.0f, 3.0f});

    glUseProgram(lightingShader);
    setInt(lightingShader, "material.diffuse", 0);
    setInt(lightingShader, "material.specular", 1);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

    /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window);

        glUseProgram(lightingShader);
        /* setVec3fv(lightingShader, "light.position", lightPos); */
        setVec3f(lightingShader, "light.direction", -0.2f, -1.0f, -0.3f);
        setVec3fv(lightingShader, "viewPos", camera->position);

        setVec3f(lightingShader, "light.ambient", 0.2f, 0.2f, 0.2f);
        setVec3f(lightingShader, "light.diffuse", 0.5f, 0.5f, 0.5f);
        setVec3f(lightingShader, "light.specular", 1.0f, 1.0f, 1.0f);

        setFloat(lightingShader, "material.shininess", 64.0f);

        // Projection matrix
        mfloat_t projection[MAT4_SIZE];
        mat4_identity(projection);
        mat4_perspective(projection, MRADIANS(camera->zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
        setMat4fv(lightingShader, "projection", projection);

        // View matrix
        mfloat_t view[MAT4_SIZE];
        getViewMatrix(camera, view);
        setMat4fv(lightingShader, "view", view);

        mfloat_t model[MAT4_SIZE];
        mat4_identity(model);
        setMat4fv(lightingShader, "model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        /* glBindVertexArray(cubeVAO); */
        /* glDrawArrays(GL_TRIANGLES, 0, 36); */

        glBindVertexArray(cubeVAO);
        for (size_t i = 0; i < TOTAL_CUBE; ++i) {
            mat4_identity(model);
            mfloat_t translated[MAT4_SIZE];
            mfloat_t rotated[MAT4_SIZE];
            mat4_translate(translated, model, cubePositions[i]);
            float angle = 20.0f * i;
            mat4_rotation_axis(rotated, (mfloat_t[]){1.0f, 0.3f, 0.5f}, MRADIANS(angle));
            mat4_multiply(model, translated, rotated);
            setMat4fv(lightingShader, "model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        /* glUseProgram(lightCubeShader); */
        /* setMat4fv(lightCubeShader, "projection", projection); */
        /* setMat4fv(lightCubeShader, "view", view); */
        /* mfloat_t translated[MAT4_SIZE]; */
        /* mfloat_t scaled[MAT4_SIZE]; */
        /* mat4_translate(translated, model, lightPos); */
        /* mat4_scale(scaled, model, (mfloat_t[]){0.2f, 0.2f, 0.2f}); */
        /* mat4_multiply(model, translated, scaled); */
        /* setMat4fv(lightCubeShader, "model", model); */

        /* glBindVertexArray(lightCubeVAO); */
        /* glDrawArrays(GL_TRIANGLES, 0, 36); */

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);
    /* glDeleteBuffers(1, &EBO); */
    glDeleteProgram(lightCubeShader);
    glDeleteProgram(lightingShader);

    free(camera);

    glfwTerminate();
    return EXIT_SUCCESS;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
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

unsigned int loadTexture(char const* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
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
        fprintf(stderr, "Texture failed to load at path: %s\n", path);
        stbi_image_free(data);
    }

    return textureID;
}
