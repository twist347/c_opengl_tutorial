#version 330 core

layout (location = 0) in vec3 a_pos;

uniform float time;

void main() {
    float c = cos(time);
    float s = sin(time);

    mat4 rot = mat4(
        c, s, 0.0, 0.0,
        -s, c, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    gl_Position = rot * vec4(a_pos, 1.0);
};