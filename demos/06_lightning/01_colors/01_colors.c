#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"

#include "ogt_util.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define SCREEN_TITLE __FILE__

#define OPENGL_MAJOR_VERSION 3
#define OPENGL_MINOR_VERSION 3

#define OBJECT_VS_SRC    "shaders/object.vert"
#define OBJECT_FS_SRC    "shaders/object.frag"

#define LIGHT_VS_SRC     "shaders/light.vert"
#define LIGHT_FS_SRC     "shaders/light.frag"

typedef struct {
    vec3 pos;
} vertex_t;

static void process_input(GLFWwindow *window);

static void mouse_callback(GLFWwindow *window, double x_pos, double y_pos);

static void render_object(GLuint shader, GLuint VAO, GLint obj_view_loc);

static void render_light(GLuint shader, GLuint VAO, GLint light_model_loc, GLint light_view_loc);

static ogt_camera_t g_camera;
static float g_delta_time = 0.f;
static float g_last_frame = 0.f;

vec3 light_pos = {1.2f, 1.f, 2.f};

int main(void) {
    int exit_code = EXIT_SUCCESS;
    GLuint obj_shader = 0, light_shader = 0;

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

    obj_shader = ogt_build_shader_path(OBJECT_VS_SRC, OBJECT_FS_SRC);
    if (!obj_shader) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    light_shader = ogt_build_shader_path(LIGHT_VS_SRC, LIGHT_FS_SRC);
    if (!light_shader) {
        exit_code = EXIT_FAILURE;
        goto cleanup;
    }

    const vertex_t vertices[] = {
        // back face (z = -0.5)
        {.pos = {-0.5f, -0.5f, -0.5f}},
        {.pos = {0.5f, -0.5f, -0.5f}},
        {.pos = {0.5f, 0.5f, -0.5f}},
        {.pos = {-0.5f, 0.5f, -0.5f}},

        // front face (z = 0.5)
        {.pos = {-0.5f, -0.5f, 0.5f}},
        {.pos = {0.5f, -0.5f, 0.5f}},
        {.pos = {0.5f, 0.5f, 0.5f}},
        {.pos = {-0.5f, 0.5f, 0.5f}},

        // left face (x = -0.5)
        {.pos = {-0.5f, -0.5f, -0.5f}},
        {.pos = {-0.5f, -0.5f, 0.5f}},
        {.pos = {-0.5f, 0.5f, 0.5f}},
        {.pos = {-0.5f, 0.5f, -0.5f}},

        // right face (x = 0.5)
        {.pos = {0.5f, -0.5f, -0.5f}},
        {.pos = {0.5f, -0.5f, 0.5f}},
        {.pos = {0.5f, 0.5f, 0.5f}},
        {.pos = {0.5f, 0.5f, -0.5f}},

        // bottom face (y = -0.5)
        {.pos = {-0.5f, -0.5f, -0.5f}},
        {.pos = {0.5f, -0.5f, -0.5f}},
        {.pos = {0.5f, -0.5f, 0.5f}},
        {.pos = {-0.5f, -0.5f, 0.5f}},

        // top face (y = 0.5)
        {.pos = {-0.5f, 0.5f, -0.5f}},
        {.pos = {0.5f, 0.5f, -0.5f}},
        {.pos = {0.5f, 0.5f, 0.5f}},
        {.pos = {-0.5f, 0.5f, 0.5f}},
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

    GLuint obj_VAO = 0, VBO = 0, EBO = 0;
    glGenVertexArrays(1, &obj_VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(obj_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) offsetof(vertex_t, pos));
    glEnableVertexAttribArray(0);

    GLuint light_VAO = 0;
    glGenVertexArrays(1, &light_VAO);

    glBindVertexArray(light_VAO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) offsetof(vertex_t, pos));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    vec3 cam_init_pos = {0.f, 0.f, 5.f};
    ogt_camera_init(&g_camera, cam_init_pos, (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT);

    mat4 model;
    glm_mat4_identity(model);

    mat4 projection;
    ogt_camera_get_projection(&g_camera, projection);

    const GLint obj_view_loc = glGetUniformLocation(obj_shader, "u_view");
    glUseProgram(obj_shader);
    glUniform3f(glGetUniformLocation(obj_shader, "u_object_color"), 1.f, 0.5f, 0.31f);
    glUniform3f(glGetUniformLocation(obj_shader, "u_light_color"), 1.f, 1.f, 1.f);
    glUniformMatrix4fv(glGetUniformLocation(obj_shader, "u_projection"), 1, GL_FALSE, (float *) projection);
    glUniformMatrix4fv(glGetUniformLocation(obj_shader, "u_model"), 1, GL_FALSE, (float *) model);

    const GLint light_model_loc = glGetUniformLocation(light_shader, "u_model");
    const GLint light_view_loc = glGetUniformLocation(light_shader, "u_view");
    glUseProgram(light_shader);
    glUniformMatrix4fv(glGetUniformLocation(light_shader, "u_projection"), 1, GL_FALSE, (float *) projection);

    glClearColor(0.f, 0.f, 0.f, 1.f);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        const float current_frame = (float) glfwGetTime();
        g_delta_time = current_frame - g_last_frame;
        g_last_frame = current_frame;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        process_input(window);

        render_light(light_shader, light_VAO, light_model_loc, light_view_loc);
        render_object(obj_shader, obj_VAO, obj_view_loc);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &obj_VAO);
    glDeleteVertexArrays(1, &light_VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

cleanup:
    if (obj_shader) {
        glDeleteProgram(obj_shader);
    }
    if (light_shader) {
        glDeleteProgram(light_shader);
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

static void render_object(GLuint shader, GLuint VAO, GLint obj_view_loc) {
    glUseProgram(shader);
    mat4 view;
    ogt_camera_get_view(&g_camera, view);
    glUniformMatrix4fv(obj_view_loc, 1, GL_FALSE, (float *) view);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
}

static void render_light(GLuint shader, GLuint VAO, GLint light_model_loc, GLint light_view_loc) {
    glUseProgram(shader);
    mat4 view;
    ogt_camera_get_view(&g_camera, view);
    glUniformMatrix4fv(light_view_loc, 1, GL_FALSE, (float *) view);
    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, light_pos);
    glm_scale(model, (vec3){0.2f, 0.2f, 0.2f});
    glUniformMatrix4fv(light_model_loc, 1, GL_FALSE, (float *) model);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
}
