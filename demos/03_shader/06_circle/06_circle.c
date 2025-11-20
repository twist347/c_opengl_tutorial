/*
draw a circle using GL_TRIANGLE_FAN
*/

#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "ogt_util.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define SCREEN_TITLE __FILE__

#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 3

#define VERTEX_SHADER_SRC     "shaders/shader.vert"
#define FRAGMENT_SHADER_SRC   "shaders/shader.frag"

#define SEGMENTS 64

typedef struct {
    GLfloat pos[3];
    GLfloat color[3];
} vertex_t;

static void process_input(GLFWwindow *window);

static void render(GLFWwindow *window, GLuint shader_program, GLuint VAO);

static bool enable_polygon_mode = false;

int main(void) {
    int exit_code = EXIT_SUCCESS;
    GLuint shader_program = 0;

    GLFWwindow *window = ogt_create_window_and_context(
        SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE,
        OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION
    );
    if (!window) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    shader_program = ogt_build_shader_program_path(VERTEX_SHADER_SRC, FRAGMENT_SHADER_SRC);
    if (!shader_program) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    const float cx = 0.f, cy = 0.f;
    const float r = 0.5f;

    vertex_t vertices[SEGMENTS + 2] = {0};

    vertices[0].pos[0] = cx;
    vertices[0].pos[1] = cy;
    vertices[0].pos[2] = 0.f;
    vertices[0].color[0] = 1.f;
    vertices[0].color[1] = 0.f;
    vertices[0].color[2] = 0.f;

    static const float double_pi = 2.f * (float) M_PI;

    for (int i = 0; i <= SEGMENTS; ++i) {
        const float segment = (float) i / (float) SEGMENTS; // [0.0 ... 1.0]
        const float angle = segment * double_pi;

        const float x = cx + r * cosf(angle);
        const float y = cy + r * sinf(angle);

        const int idx = i + 1;
        vertices[idx].pos[0] = x;
        vertices[idx].pos[1] = y;
        vertices[idx].pos[2] = 0.f;

        vertices[idx].color[0] = 1.f;
        vertices[idx].color[1] = 0.f;
        vertices[idx].color[2] = 0.f;
    }

    GLuint VAO = 0, VBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) offsetof(vertex_t, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) offsetof(vertex_t, color));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(1.f, 1.f, 1.f, 1.f);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        render(window, shader_program, VAO);

        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

cleanup:
    if (shader_program) {
        glDeleteProgram(shader_program);
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

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        enable_polygon_mode = !enable_polygon_mode;
    }
}

static void render(GLFWwindow *window, GLuint shader_program, GLuint VAO) {
    glClear(GL_COLOR_BUFFER_BIT);

    if (enable_polygon_mode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glUseProgram(shader_program);
    glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLE_FAN, 0, SEGMENTS + 2);

    glfwSwapBuffers(window);
}
