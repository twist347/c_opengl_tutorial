/*
draw a circle using fragment shader
*/

#include <stdlib.h>

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

static void render(
    GLFWwindow *window,
    GLuint shader_program, GLuint VAO, GLuint EBO,
    GLint loc_center, GLint loc_radius
);

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
        {.pos = {-0.5f, -0.5f}, .color = {1.f, 0.f, 0.f}},
        {.pos = {0.5f, -0.5f}, .color = {1.f, 0.f, 0.f}},
        {.pos = {0.5f, 0.5f}, .color = {1.f, 0.f, 0.f}},
        {.pos = {-0.5f, 0.5f}, .color = {1.f, 0.f, 0.f}},
    };

    const GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
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

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) offsetof(vertex_t, color));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(0.5f, 0.5f, 0.5f, 1.f);

    glUseProgram(shader);
    const GLint loc_center = glGetUniformLocation(shader, "u_center");
    const GLint loc_radius = glGetUniformLocation(shader, "u_radius");

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        render(window, shader, VAO, EBO, loc_center, loc_radius);

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

static void render(
    GLFWwindow *window,
    GLuint shader_program, GLuint VAO, GLuint EBO,
    GLint loc_center, GLint loc_radius
) {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glUniform2f(loc_center, 0.f, 0.f);
    glUniform1f(loc_radius, 0.25f);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
}
