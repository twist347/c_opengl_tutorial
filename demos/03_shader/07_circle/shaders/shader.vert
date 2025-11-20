#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;

out vec3 color;
out vec3 pos;

void main() {
    color = a_color;
    pos = a_pos;
    gl_Position = vec4(a_pos, 1.0);
}
