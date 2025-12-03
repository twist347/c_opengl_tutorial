/*
moving and scaling triangle using keys
sending offset(up, down, left, right) and scale(q, e) using uniforms
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

#define MOVE_SPEED     0.01f
#define SCALE_SPEED    0.01f

typedef struct {
    GLfloat pos[3];
} vertex_t;

static void process_input(GLFWwindow *window);

static void render(GLFWwindow *window, GLuint shader_program, GLuint VAO, GLint loc_time, GLint loc_offset, GLint loc_scale);

static float offset_x = 0.f;
static float offset_y = 0.f;
static float scale = 1.f;

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
    const GLint loc_offset = glGetUniformLocation(shader, "u_offset");
    const GLint loc_scale = glGetUniformLocation(shader, "u_scale");

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        render(window, shader, VAO, loc_time, loc_offset, loc_scale);

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

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        offset_x -= MOVE_SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        offset_x += MOVE_SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        offset_y += MOVE_SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        offset_y -= MOVE_SPEED;
    }

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        scale -= SCALE_SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        scale += SCALE_SPEED;
    }
}

static void render(GLFWwindow *window, GLuint shader_program, GLuint VAO, GLint loc_time, GLint loc_offset, GLint loc_scale) {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);

    const float cur_time = (float) glfwGetTime();
    glUniform1f(loc_time, cur_time);

    glUniform2f(loc_offset, offset_x, offset_y);

    glUniform1f(loc_scale, scale);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
}
