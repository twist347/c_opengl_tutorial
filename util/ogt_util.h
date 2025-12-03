#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

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

GLuint ogt_build_shader_src(const char *vertex_shader_src, const char *fragment_shader_src);

GLuint ogt_build_shader_path(const char *vertex_shader_path, const char *fragment_shader_path);

GLFWwindow *ogt_create_window_and_context(int width, int height, const char *title, int major, int minor);
