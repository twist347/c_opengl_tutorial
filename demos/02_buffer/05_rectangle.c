/*
draw rectangle using 2 triangles
*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define SCREEN_TITLE __FILE__

#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 3

#define LOG_LEN 1024

typedef struct {
    GLfloat pos[3];
} vertex_t;

static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

static void process_input(GLFWwindow *window);

static GLFWwindow *create_window_and_context(int width, int height, const char *title);

static int init_glad(void);

static void init_viewport(GLFWwindow *window);

static void print_gl_info(void);

static GLuint compile_shader(GLenum type, const char *source);

static GLuint create_shader_program(GLuint vertex_shader, GLuint fragment_shader);

static GLuint build_program(const char *vertex_shader_src, const char *fragment_shader_src);

static void render(GLuint shader_program, GLuint VAO, GLFWwindow *window);

static const char *vertex_shader_source =
        "#version 330 core\n"

        "layout (location = 0) in vec3 a_pos;\n"

        "void main() {\n"
        "    gl_Position = vec4(a_pos, 1.0);\n"
        "}\n";

static const char *fragment_shader_source =
        "#version 330 core\n"

        "out vec4 frag_color;\n"

        "void main() {\n"
        "   frag_color = vec4(0.0, 0.0, 1.0, 1.0);\n"
        "}\n";

int main(void) {
    int exit_code = EXIT_SUCCESS;
    GLuint shader_program = 0;

    GLFWwindow *window = create_window_and_context(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);
    if (!window) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    if (!init_glad()) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    init_viewport(window);

    print_gl_info();

    shader_program = build_program(vertex_shader_source, fragment_shader_source);
    if (!shader_program) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    const vertex_t vertices[] = {
        {.pos = {0.5f, 0.5f, 0.f}},
        {.pos = {0.5f, -0.5f, 0.f}},
        {.pos = {-0.5f, -0.5f, 0.f}},
        {.pos = {-0.5f, 0.5f, 0.f}}
    };

    const GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) offsetof(vertex_t, pos));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(1.f, 0.f, 0.f, 1.f);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        render(shader_program, VAO, window);

        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

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

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    (void) window;
    glViewport(0, 0, width, height);
}

static void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static GLFWwindow *create_window_and_context(int width, int height, const char *title) {
    if (!glfwInit()) {
        fprintf(stderr, "[ERROR]: Failed to initialize GLFW\n");
        return NULL;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        fprintf(stderr, "[ERROR]: failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    return window;
}

static int init_glad(void) {
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "[ERROR]: failed to initialize GLAD\n");
        return 0;
    }
    return 1;
}

static void init_viewport(GLFWwindow *window) {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    framebuffer_size_callback(window, width, height);
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

static GLuint compile_shader(GLenum type, const char *source) {
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char info_log[LOG_LEN];
        glGetShaderInfoLog(shader, LOG_LEN, NULL, info_log);
        const char *shader_type;
        if (type == GL_VERTEX_SHADER) {
            shader_type = "vertex";
        } else if (type == GL_FRAGMENT_SHADER) {
            shader_type = "fragment";
        } else {
            shader_type = "unknown";
        }
        fprintf(stderr, "[ERROR]: failed to compile %s shader: %s\n", shader_type, info_log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static GLuint create_shader_program(GLuint vertex_shader, GLuint fragment_shader) {
    if (!vertex_shader || !fragment_shader) {
        return 0;
    }
    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint success = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        char info_log[LOG_LEN];
        glGetProgramInfoLog(program, LOG_LEN, NULL, info_log);
        fprintf(stderr, "[ERROR]: failed to link shader program: %s\n", info_log);
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

static GLuint build_program(const char *vertex_shader_src, const char *fragment_shader_src) {
    GLuint vertex_shader = 0, fragment_shader = 0, program = 0;

    vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    if (!vertex_shader) {
        goto cleanup;
    }

    fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
    if (!fragment_shader) {
        goto cleanup;
    }

    program = create_shader_program(vertex_shader, fragment_shader);

cleanup:
    if (fragment_shader) {
        glDeleteShader(fragment_shader);
    }
    if (vertex_shader) {
        glDeleteShader(vertex_shader);
    }
    return program;
}

static void render(GLuint shader_program, GLuint VAO, GLFWwindow *window) {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
}
