/*
texture effects demo

    0          - orig
    1          - grayscale
    2          - invert
    3          - sepia
    4          - brightness/contrast/saturation
    5          - vignette
    6          - waves (UV distortion)
    7          - scroll
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "ogt_util.h"

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080
#define SCREEN_TITLE  __FILE__

#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 3

#define VERTEX_SHADER_SRC   "shaders/shader.vert"
#define FRAGMENT_SHADER_SRC "shaders/shader.frag"

#define TEXTURE_SRC "textures/img1.png"

typedef struct {
    GLfloat pos[3];
    GLfloat color[3];
    GLfloat tex[2];
} vertex_t;

static void process_input(GLFWwindow *window);

static void render(GLFWwindow *window, GLuint shader_program, GLuint VAO, GLuint texture, GLint mode_loc,
                   GLint time_loc);

static GLuint load_texture(const char *filename);

static GLint g_mode = 0;

int main(void) {
    int exit_code = EXIT_SUCCESS;
    GLuint shader_program = 0, texture = 0;

    GLFWwindow *window = ogt_create_window_and_context(
        SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE,
        OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION
    );
    if (!window) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    shader_program = ogt_build_shader_program_path(VERTEX_SHADER_SRC, FRAGMENT_SHADER_SRC);
    if (!shader_program) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    glUseProgram(shader_program);
    glUniform1i(glGetUniformLocation(shader_program, "tex0"), 0);

    const vertex_t vertices[] = {
        {.pos = {0.5f, 0.5f, 0.f}, .color = {1.f, 0.f, 0.f}, .tex = {1.f, 1.f}}, // top right
        {.pos = {0.5f, -0.5f, 0.f}, .color = {0.f, 1.f, 0.f}, .tex = {1.f, 0.f}}, // bottom right
        {.pos = {-0.5f, -0.5f, 0.f}, .color = {0.f, 0.f, 1.f}, .tex = {0.f, 0.f}}, // bottom left
        {.pos = {-0.5f, 0.5f, 0.f}, .color = {1.f, 1.f, 1.f}, .tex = {0.f, 1.f}} // top left
    };

    const GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
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

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) offsetof(vertex_t, tex));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    texture = load_texture(TEXTURE_SRC);
    if (!texture) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    glUseProgram(shader_program);
    const GLint mode_loc = glGetUniformLocation(shader_program, "mode");
    const GLint time_loc = glGetUniformLocation(shader_program, "time");

    glClearColor(0.1f, 0.1f, 0.1f, 1.f);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        render(window, shader_program, VAO, texture, mode_loc, time_loc);

        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

cleanup:
    if (shader_program) {
        glDeleteProgram(shader_program);
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

    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) g_mode = 0;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) g_mode = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) g_mode = 2;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) g_mode = 3;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) g_mode = 4;
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) g_mode = 5;
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) g_mode = 6;
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) g_mode = 7;
}

static void render(GLFWwindow *window, GLuint shader_program, GLuint VAO, GLuint texture, GLint mode_loc,
                   GLint time_loc) {
    (void) window;

    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);

    const float t = (float) glfwGetTime();

    glUniform1i(mode_loc, g_mode);
    glUniform1f(time_loc, t);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
}

static GLuint load_texture(const char *filename) {
    GLuint texture = 0;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int width, height, nr_channels;
    stbi_set_flip_vertically_on_load(1);
    stbi_uc *data = stbi_load(filename, &width, &height, &nr_channels, 0);
    if (!data) {
        printf("[ERROR]: failed to load image '%s'.\n", filename);
        glDeleteTextures(1, &texture);
        return 0;
    }

    GLenum format;
    if (nr_channels == 1) {
        format = GL_RED;
    } else if (nr_channels == 3) {
        format = GL_RGB;
    } else if (nr_channels == 4) {
        format = GL_RGBA;
    } else {
        printf("[ERROR]: unsupported channel count (%d) in '%s'.\n", nr_channels, filename);
        stbi_image_free(data);
        glDeleteTextures(1, &texture);
        return 0;
    }

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 format,
                 width, height,
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 data);

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}
