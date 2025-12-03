/*
render a texture
*/

#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "ogt_util.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define SCREEN_TITLE __FILE__

#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 3

#define VERTEX_SHADER_SRC     "shaders/shader.vert"
#define FRAGMENT_SHADER_SRC   "shaders/shader.frag"

#define TEXTURE_SRC    "textures/img2.png"

typedef struct {
    GLfloat pos[3];
    GLfloat tex[2];
} vertex_t;

static void process_input(GLFWwindow *window);

static void render(GLFWwindow *window, GLuint shader_program, GLuint VAO, GLuint texture);

int main(void) {
    int exit_code = EXIT_SUCCESS;
    GLuint shader = 0, texture = 0;

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

    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "u_tex0"), 0);

    const vertex_t vertices[] = {
        {.pos = {-0.5f, -0.5f, 0.f}, .tex = {0.f, 0.f}}, // left
        {.pos = {0.5f, -0.5f, 0.f}, .tex = {1.f, 0.f}}, // right
        {.pos = {0.f, 0.5f, 0.f}, .tex = {0.5f, 1.f}} // top
    };

    GLuint VAO = 0, VBO = 0;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) offsetof(vertex_t, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) offsetof(vertex_t, tex));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // load texture

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nr_channels;

    stbi_set_flip_vertically_on_load(1);
    stbi_uc *data = stbi_load(TEXTURE_SRC, &width, &height, &nr_channels, 0);
    if (!data) {
        exit_code = EXIT_FAILURE;
        printf("[ERROR]: failed to load image.\n");
        goto cleanup;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glClearColor(0.5f, 0.5f, 0.5f, 1.f);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        render(window, shader, VAO, texture);

        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

cleanup:
    if (shader) {
        glDeleteProgram(shader);
    }
    if (texture) {
        glDeleteTextures(1, &texture);
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

static void render(GLFWwindow *window, GLuint shader_program, GLuint VAO, GLuint texture) {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
}
