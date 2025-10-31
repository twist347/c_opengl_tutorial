/*
creating a window using glfw and glad for opengl
*/

#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define SCREEN_TITLE "01_window"

#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 3

static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

static void process_input(GLFWwindow *window);

static void print_gl_info(void);

int main(void) {
    if (!glfwInit()) {
        fprintf(stderr, "[ERROR]: Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE, NULL, NULL);
    if (!window) {
        fprintf(stderr, "[ERROR]: failed to create GLFW window\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        fprintf(stderr, "[ERROR]: failed to initialize GLAD\n");
        return EXIT_FAILURE;
    }

    print_gl_info();

    glClearColor(1.f, 1.f, 1.f, 1.f);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    (void) window;
    glViewport(0, 0, width, height);
}

static void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static void print_gl_info(void) {
    GLint major = 0, minor = 0, profile = 0, n_ext = 0;
    const char *vendor = (const char *) glGetString(GL_VENDOR);
    const char *renderer = (const char *) glGetString(GL_RENDERER);
    const char *version = (const char *) glGetString(GL_VERSION);
    const char *glsl = (const char *) glGetString(GL_SHADING_LANGUAGE_VERSION);

    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
    glGetIntegerv(GL_NUM_EXTENSIONS, &n_ext);

    const char *profile_str =
            profile & GL_CONTEXT_CORE_PROFILE_BIT
                ? "Core"
                : profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT
                      ? "Compatibility"
                      : "Unknown";

    fprintf(stdout, "---------------- OpenGL context ----------------\n");
    fprintf(stdout, "Version:    %d.%d (%s)\n", major, minor, version ? version : "?");
    fprintf(stdout, "GLSL:       %s\n", glsl ? glsl : "?");
    fprintf(stdout, "Vendor:     %s\n", vendor ? vendor : "?");
    fprintf(stdout, "Renderer:   %s\n", renderer ? renderer : "?");
    fprintf(stdout, "Profile:    %s\n", profile_str);
    fprintf(stdout, "Extensions: %d\n", n_ext);
}
