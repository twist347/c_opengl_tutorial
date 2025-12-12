#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "cglm/cglm.h"

#define OGT_ARR_LEN(arr)    (sizeof((arr)) / sizeof((arr)[0]))

// colors stuff

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} ogt_color_t;

_Static_assert(sizeof(ogt_color_t) == 4, "ogt_color_t must be 4 bytes (RGBA)");

#define OGT_BLACK        (ogt_color_t){0, 0, 0, 255}
#define OGT_WHITE        (ogt_color_t){255, 255, 255, 255}
#define OGT_RED          (ogt_color_t){255, 0, 0, 255}
#define OGT_GREEN        (ogt_color_t){0, 255, 0, 255}
#define OGT_BLUE         (ogt_color_t){0, 0, 255, 255}
#define OGT_YELLOW       (ogt_color_t){255, 255, 0, 255}
#define OGT_CYAN         (ogt_color_t){0, 255, 255, 255}
#define OGT_MAGENTA      (ogt_color_t){255, 0, 255, 255}
#define OGT_GRAY         (ogt_color_t){128, 128, 128, 255}

// camera stuff

typedef struct {
    vec3 pos;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 world_up;

    float yaw;
    float pitch;

    float speed;
    float sens;

    float fov;
    float z_near;
    float z_far;
    float aspect;
} ogt_camera_t;

typedef enum {
    OGT_CAMERA_FORWARD,
    OGT_CAMERA_BACKWARD,
    OGT_CAMERA_LEFT,
    OGT_CAMERA_RIGHT
} ogt_camera_movement;

void ogt_camera_init(ogt_camera_t *cam, vec3 position, float aspect);

// если нужно поменять проекцию (например, при resize окна)
void ogt_camera_set_perspective(ogt_camera_t *cam, float fov, float z_near, float z_far, float aspect);

void ogt_camera_get_view(ogt_camera_t *cam, mat4 dest);

void ogt_camera_get_projection(const ogt_camera_t *cam, mat4 dest);

void ogt_camera_process_keyboard(ogt_camera_t *cam, ogt_camera_movement direction, float dt);

void ogt_camera_process_mouse(ogt_camera_t *cam, float x_offset, float y_offset, int constrain_pitch);

// shaders stuff

GLuint ogt_build_shader_src(const char *vertex_shader_src, const char *fragment_shader_src);

GLuint ogt_build_shader_path(const char *vertex_shader_path, const char *fragment_shader_path);

// window stuff

GLFWwindow *ogt_create_window_and_context(int width, int height, const char *title, int major, int minor);
