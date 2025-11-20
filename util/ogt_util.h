#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

GLuint ogt_build_shader_program_src(const char *vertex_shader_src, const char *fragment_shader_src);

GLuint ogt_build_shader_program_path(const char *vertex_shader_path, const char *fragment_shader_path);

GLFWwindow *ogt_create_window_and_context(int width, int height, const char *title, int major, int minor);
