#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec4 a_color;

out vec4 color;

uniform mat4 trans;

void main() {
    gl_Position = trans * vec4(a_pos, 1.0);
    color = a_color;
};