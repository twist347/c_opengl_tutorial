/*
rotating triangle
sending rotating mat4 using uniform
*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

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

typedef struct {
    GLfloat pos[3];
} vertex_t;

static void process_input(GLFWwindow *window);

static void render(GLFWwindow *window, GLuint shader_program, GLuint VAO, GLint loc_time);

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
        {.pos = {-0.5f, -0.5f, 0.f}}, // left
        {.pos = {0.5f, -0.5f, 0.f}},  // right
        {.pos = {0.f, 0.5f, 0.f}} // top
    };

    GLuint VAO = 0, VBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) offsetof(vertex_t, pos));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(0.5f, 0.5f, 0.5f, 1.f);

    glUseProgram(shader);
    const GLint loc_time = glGetUniformLocation(shader, "u_time");

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        render(window, shader, VAO, loc_time);

        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

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

static void render(GLFWwindow *window, GLuint shader_program, GLuint VAO, GLint loc_time) {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);

    const float cur_time = (float) glfwGetTime();
    glUniform1f(loc_time, cur_time);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
}
