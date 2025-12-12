#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

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

static void mouse_callback(GLFWwindow *window, double x_pos, double y_pos);

static void render(
    GLFWwindow *window,
    GLuint shader, GLuint VAO,
    GLint model_loc, GLint view_loc,
    vec3 cubes_pos[], size_t n_cubes
);

static bool enable_depth_test = true;

static ogt_camera_t g_camera;
static float g_delta_time = 0.f;
static float g_last_frame = 0.f;

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

    glfwSetWindowUserPointer(window, &g_camera);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

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

    vec3 cubes_pos[] = {
        {0.f, 0.f, 0.f},
        {2.f, 0.f, 1.f},
        {-2.f, 0.f, 2.f},
        {0.f, 2.f, 3.f},
        {0.f, -2.f, 4.f},
    };

    const size_t n_cubes = OGT_ARR_LEN(cubes_pos);

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

    glUseProgram(shader);

    const GLint model_loc = glGetUniformLocation(shader, "u_model");
    const GLint view_loc = glGetUniformLocation(shader, "u_view");
    const GLint proj_loc = glGetUniformLocation(shader, "u_projection");

    glClearColor(0.5f, 0.5f, 0.5f, 1.f);

    if (enable_depth_test) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    vec3 cam_init_pos = {0.f, 0.f, 5.f};
    ogt_camera_init(&g_camera, cam_init_pos, (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT);

    mat4 projection;
    ogt_camera_get_projection(&g_camera, projection);
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, (float *) projection);

    while (!glfwWindowShouldClose(window)) {
        const float current_frame = (float) glfwGetTime();
        g_delta_time = current_frame - g_last_frame;
        g_last_frame = current_frame;

        process_input(window);

        if (enable_depth_test) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        render(window, shader, VAO, model_loc, view_loc, cubes_pos, n_cubes);

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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        ogt_camera_process_keyboard(&g_camera, OGT_CAMERA_FORWARD, g_delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        ogt_camera_process_keyboard(&g_camera, OGT_CAMERA_BACKWARD, g_delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        ogt_camera_process_keyboard(&g_camera, OGT_CAMERA_LEFT, g_delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        ogt_camera_process_keyboard(&g_camera, OGT_CAMERA_RIGHT, g_delta_time);
    }

    static bool depth_key_pressed = false;
    const int state = glfwGetKey(window, GLFW_KEY_Z);
    if (state == GLFW_PRESS && !depth_key_pressed) {
        enable_depth_test = !enable_depth_test;
        depth_key_pressed = true;
    }
    if (state == GLFW_RELEASE) {
        depth_key_pressed = false;
    }
}

static void mouse_callback(GLFWwindow *window, double x_pos, double y_pos) {
    static double last_x = SCREEN_WIDTH / 2.0;
    static double last_y = SCREEN_HEIGHT / 2.0;
    static bool first_mouse = true;

    ogt_camera_t *camera = glfwGetWindowUserPointer(window);
    if (!camera) {
        return;
    }

    if (first_mouse) {
        last_x = x_pos;
        last_y = y_pos;
        first_mouse = false;
    }

    const float x_offset = (float) (x_pos - last_x);
    const float y_offset = (float) (last_y - y_pos);

    last_x = x_pos;
    last_y = y_pos;

    ogt_camera_process_mouse(camera, x_offset, y_offset, 1);
}

static void render(
    GLFWwindow *window,
    GLuint shader, GLuint VAO,
    GLint model_loc, GLint view_loc,
    vec3 cubes_pos[], size_t n_cubes
) {
    (void) window;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    glBindVertexArray(VAO);

    const float time = (float) glfwGetTime();

    mat4 view;
    ogt_camera_get_view(&g_camera, view);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float *) view);

    for (size_t i = 0; i < n_cubes; ++i) {
        mat4 model;
        glm_mat4_identity(model);

        const float angle = glm_rad(50.f) * time + glm_rad(20.f) * (float) i;

        glm_translate(model, cubes_pos[i]);
        glm_rotate(model, angle, (vec3){0.5f, 1.f, 1.f});

        glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float *) model);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
    }

    glfwSwapBuffers(window);
}
