#include "ogt_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

#define LOG_LEN 1024

static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

static int init_glad(void);

static void init_viewport(GLFWwindow *window);

static GLuint compile_shader(GLenum type, const char *source);

static GLuint create_shader(GLuint vertex_shader, GLuint fragment_shader);

static void print_info(void);

static char *ogt_read_file(const char *filepath, size_t *out_size);

static char *ogt_read_file_text(const char *filepath);

static void ogt_camera_update_vectors(ogt_camera_t *cam);

void ogt_camera_init(ogt_camera_t *cam, vec3 position, float aspect) {
    glm_vec3_copy(position, cam->pos);

    cam->yaw = -90.f;
    cam->pitch = 0.f;

    cam->speed = 2.5f;
    cam->sens = 0.1f;

    cam->fov = 45.f;
    cam->z_near = 0.1f;
    cam->z_far = 100.f;
    cam->aspect = aspect;

    vec3 world_up = {0.f, 1.f, 0.f};
    glm_vec3_copy(world_up, cam->world_up);

    ogt_camera_update_vectors(cam);
}

void ogt_camera_set_perspective(ogt_camera_t *cam, float fov, float z_near, float z_far, float aspect) {
    cam->fov = fov;
    cam->z_near = z_near;
    cam->z_far = z_far;
    cam->aspect = aspect;
}

void ogt_camera_get_view(ogt_camera_t *cam, mat4 dest) {
    vec3 center;
    glm_vec3_add(cam->pos, cam->front, center);
    glm_lookat(cam->pos, center, cam->up, dest);
}

void ogt_camera_get_projection(const ogt_camera_t *cam, mat4 dest) {
    glm_perspective(glm_rad(cam->fov), cam->aspect, cam->z_near, cam->z_far, dest);
}

void ogt_camera_process_keyboard(ogt_camera_t *cam, ogt_camera_movement dir, float dt) {
    const float velocity = cam->speed * dt;
    vec3 tmp;

    switch (dir) {
        case OGT_CAMERA_FORWARD:
            glm_vec3_scale(cam->front, velocity, tmp);
            glm_vec3_add(cam->pos, tmp, cam->pos);
            break;
        case OGT_CAMERA_BACKWARD:
            glm_vec3_scale(cam->front, velocity, tmp);
            glm_vec3_sub(cam->pos, tmp, cam->pos);
            break;
        case OGT_CAMERA_LEFT:
            glm_vec3_scale(cam->right, velocity, tmp);
            glm_vec3_sub(cam->pos, tmp, cam->pos);
            break;
        case OGT_CAMERA_RIGHT:
            glm_vec3_scale(cam->right, velocity, tmp);
            glm_vec3_add(cam->pos, tmp, cam->pos);
            break;
        default:
            break;
    }
}

void ogt_camera_process_mouse(ogt_camera_t *cam, float x_offset, float y_offset, int constrain_pitch) {
    x_offset *= cam->sens;
    y_offset *= cam->sens;

    cam->yaw += x_offset;
    cam->pitch += y_offset;

    if (constrain_pitch) {
        if (cam->pitch > 89.f) cam->pitch = 89.f;
        if (cam->pitch < -89.f) cam->pitch = -89.f;
    }

    ogt_camera_update_vectors(cam);
}

GLuint ogt_build_shader_src(const char *vertex_shader_src, const char *fragment_shader_src) {
    GLuint vertex_shader = 0, fragment_shader = 0, program = 0;

    vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    if (!vertex_shader) {
        goto cleanup;
    }

    fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
    if (!fragment_shader) {
        goto cleanup;
    }

    program = create_shader(vertex_shader, fragment_shader);

cleanup:
    if (fragment_shader) {
        glDeleteShader(fragment_shader);
    }
    if (vertex_shader) {
        glDeleteShader(vertex_shader);
    }
    return program;
}

GLuint ogt_build_shader_path(const char *vertex_shader_path, const char *fragment_shader_path) {
    char *vertex_shader_src = NULL;
    char *fragment_shader_src = NULL;
    GLuint program = 0;

    vertex_shader_src = ogt_read_file_text(vertex_shader_path);
    if (!vertex_shader_src) {
        goto cleanup;
    }

    fragment_shader_src = ogt_read_file_text(fragment_shader_path);
    if (!fragment_shader_src) {
        goto cleanup;
    }

    program = ogt_build_shader_src(vertex_shader_src, fragment_shader_src);

cleanup:
    if (fragment_shader_src) {
        free(fragment_shader_src);
    }
    if (vertex_shader_src) {
        free(vertex_shader_src);
    }
    return program;
}

GLFWwindow *ogt_create_window_and_context(int width, int height, const char *title, int major, int minor) {
    if (!glfwInit()) {
        fprintf(stderr, "[ERROR]: Failed to initialize GLFW\n");
        return NULL;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
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

    if (!init_glad()) {
        fprintf(stderr, "[ERROR]: failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return NULL;
    }

    init_viewport(window);

    print_info();

    return window;
}

static void ogt_camera_update_vectors(ogt_camera_t *cam) {
    vec3 front;

    const float yaw_rad = glm_rad(cam->yaw);
    const float pitch_rad = glm_rad(cam->pitch);

    front[0] = cosf(yaw_rad) * cosf(pitch_rad);
    front[1] = sinf(pitch_rad);
    front[2] = sinf(yaw_rad) * cosf(pitch_rad);

    glm_vec3_normalize_to(front, cam->front);

    // right = normalize(cross(front, world_up))
    glm_vec3_cross(cam->front, cam->world_up, cam->right);
    glm_vec3_normalize(cam->right);

    // up = normalize(cross(right, front))
    glm_vec3_cross(cam->right, cam->front, cam->up);
    glm_vec3_normalize(cam->up);
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    (void) window;
    glViewport(0, 0, width, height);
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

static GLuint create_shader(GLuint vertex_shader, GLuint fragment_shader) {
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

static void print_info(void) {
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

static char *ogt_read_file(const char *filepath, size_t *out_size) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        fprintf(stderr, "[ERROR]: Cannot open file %s\n", filepath);
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "[ERROR]: fseek failed for file %s\n", filepath);
        fclose(file);
        return NULL;
    }

    const long len = ftell(file);
    if (len < 0) {
        fprintf(stderr, "[ERROR]: ftell failed for file %s\n", filepath);
        fclose(file);
        return NULL;
    }

    rewind(file);

    char *buffer = malloc(len);
    if (!buffer) {
        fprintf(stderr, "[ERROR]: Memory allocation failed for file %s\n", filepath);
        fclose(file);
        return NULL;
    }

    const size_t read_len = fread(buffer, 1, len, file);
    if (read_len != (size_t) len) {
        fprintf(stderr, "[ERROR]: Read only %zu of %ld bytes from file %s\n",
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

static char *ogt_read_file_text(const char *filepath) {
    size_t size;
    char *buffer = ogt_read_file(filepath, &size);
    if (!buffer) {
        return NULL;
    }

    // Resize to add null terminator
    char *text_buffer = realloc(buffer, size + 1);
    if (!text_buffer) {
        free(buffer);
        return NULL;
    }

    text_buffer[size] = '\0';

    return text_buffer;
}
