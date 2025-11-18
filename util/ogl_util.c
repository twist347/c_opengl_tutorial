#include "ogl_util.h"

#include <stdio.h>
#include <stdlib.h>

char *ogl_read_file(const char *filepath, size_t *out_size) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filepath);
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "Error: fseek failed for file %s\n", filepath);
        fclose(file);
        return NULL;
    }

    const long len = ftell(file);
    if (len < 0) {
        fprintf(stderr, "Error: ftell failed for file %s\n", filepath);
        fclose(file);
        return NULL;
    }

    rewind(file);

    char *buffer = malloc(len);
    if (!buffer) {
        fprintf(stderr, "Error: Memory allocation failed for file %s\n", filepath);
        fclose(file);
        return NULL;
    }

    const size_t read_len = fread(buffer, 1, len, file);
    if (read_len != (size_t) len) {
        fprintf(stderr, "Error: Read only %zu of %ld bytes from file %s\n",
                read_len, len, filepath);
        free(buffer);
        fclose(file);
        return NULL;
    }

    if (out_size) {
        *out_size = read_len;
    }

    fclose(file);
    return buffer;
}

char* ogl_read_file_text(const char* filepath) {
    size_t size;
    char* buffer = ogl_read_file(filepath, &size);
    if (!buffer) {
        return NULL;
    }

    // Resize to add null terminator
    char* text_buffer = realloc(buffer, size + 1);
    if (!text_buffer) {
        free(buffer);
        return NULL;
    }

    text_buffer[size] = '\0';

    return text_buffer;
}
