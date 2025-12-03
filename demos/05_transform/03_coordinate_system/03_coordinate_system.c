#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"

#include "ogt_util.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define SCREEN_TITLE __FILE__

#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 3

#define VERTEX_SHADER_SRC     "shaders/shader.vert"
#define FRAGMENT_SHADER_SRC   "shaders/shader.frag"

typedef struct {
    vec3 pos;
    ogt_color_t color;
} vertex_t;

static void process_input(GLFWwindow *window);

static void render(GLFWwindow *window, GLuint shader, GLuint VAO, GLint model_loc);

int main(void) {
    int exit_code = EXIT_SUCCESS;
    GLuint shader = 0;

    GLFWwindow *window = ogt_create_window_and_context(
        SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE,
        OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION
    );
    if (!window) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    shader = ogt_build_shader_path(VERTEX_SHADER_SRC, FRAGMENT_SHADER_SRC);
    if (!shader) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    const vertex_t vertices[] = {
        // back face (z = -0.5) - OGT_RED
        {.pos = {-0.5f, -0.5f, -0.5f}, .color = OGT_RED},
        {.pos = {0.5f, -0.5f, -0.5f}, .color = OGT_RED},
        {.pos = {0.5f, 0.5f, -0.5f}, .color = OGT_RED},
        {.pos = {-0.5f, 0.5f, -0.5f}, .color = OGT_RED},

        // front face (z = 0.5) - OGT_GREEN
        {.pos = {-0.5f, -0.5f, 0.5f}, .color = OGT_GREEN},
        {.pos = {0.5f, -0.5f, 0.5f}, .color = OGT_GREEN},
        {.pos = {0.5f, 0.5f, 0.5f}, .color = OGT_GREEN},
        {.pos = {-0.5f, 0.5f, 0.5f}, .color = OGT_GREEN},

        // left face (x = -0.5) - OGT_BLUE
        {.pos = {-0.5f, -0.5f, -0.5f}, .color = OGT_BLUE},
        {.pos = {-0.5f, -0.5f, 0.5f}, .color = OGT_BLUE},
        {.pos = {-0.5f, 0.5f, 0.5f}, .color = OGT_BLUE},
        {.pos = {-0.5f, 0.5f, -0.5f}, .color = OGT_BLUE},

        // right face (x = 0.5) - OGT_YELLOW
        {.pos = {0.5f, -0.5f, -0.5f}, .color = OGT_YELLOW},
        {.pos = {0.5f, -0.5f, 0.5f}, .color = OGT_YELLOW},
        {.pos = {0.5f, 0.5f, 0.5f}, .color = OGT_YELLOW},
        {.pos = {0.5f, 0.5f, -0.5f}, .color = OGT_YELLOW},

        // bottom face (y = -0.5) - OGT_CYAN
        {.pos = {-0.5f, -0.5f, -0.5f}, .color = OGT_CYAN},
        {.pos = {0.5f, -0.5f, -0.5f}, .color = OGT_CYAN},
        {.pos = {0.5f, -0.5f, 0.5f}, .color = OGT_CYAN},
        {.pos = {-0.5f, -0.5f, 0.5f}, .color = OGT_CYAN},

        // top face (y = 0.5) - OGT_MAGENTA
        {.pos = {-0.5f, 0.5f, -0.5f}, .color = OGT_MAGENTA},
        {.pos = {0.5f, 0.5f, -0.5f}, .color = OGT_MAGENTA},
        {.pos = {0.5f, 0.5f, 0.5f}, .color = OGT_MAGENTA},
        {.pos = {-0.5f, 0.5f, 0.5f}, .color = OGT_MAGENTA},
    };

    const GLuint indices[] = {
        // back face
        0, 1, 2,
        2, 3, 0,

        // front face
        4, 5, 6,
        6, 7, 4,

        // left face
        8, 9, 10,
        10, 11, 8,

        // right face
        12, 13, 14,
        14, 15, 12,

        // bottom face
        16, 17, 18,
        18, 19, 16,

        // top face
        20, 21, 22,
        22, 23, 20
    };

    GLuint VAO = 0, VBO = 0, EBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) offsetof(vertex_t, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertex_t), (void *) offsetof(vertex_t, color));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    mat4 view;
    glm_mat4_identity(view);
    glm_translate(view, (vec3){0.f, 0.f, -3.f});

    mat4 projection;
    glm_perspective(glm_rad(45.f), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT, 0.1f, 100.f, projection);

    glUseProgram(shader);

    const GLint model_loc = glGetUniformLocation(shader, "u_model");
    glUniformMatrix4fv(glGetUniformLocation(shader, "u_view"), 1, GL_FALSE, (float *) view);
    glUniformMatrix4fv(glGetUniformLocation(shader, "u_projection"), 1, GL_FALSE, (float *) projection);

    glClearColor(0.5f, 0.5f, 0.5f, 1.f);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        render(window, shader, VAO, model_loc);

        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

cleanup:
    if (shader) {
        glDeleteProgram(shader);
    }
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    return exit_code;
}

static void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void render(GLFWwindow *window, GLuint shader, GLuint VAO, GLint model_loc) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    mat4 model;
    glm_mat4_identity(model);
    glm_rotate(model, glm_rad(50.f) * (float) glfwGetTime(), (vec3){0.5f, 1.f, 1.f});
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *) model);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
}
