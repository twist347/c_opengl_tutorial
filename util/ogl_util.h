#pragma once

#include <stddef.h>

char* ogl_read_file(const char* filepath, size_t* out_size);

char* ogl_read_file_text(const char* filepath);
