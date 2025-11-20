#version 330 core

layout (location = 0) in vec3 a_pos;

uniform vec2 offset;
uniform float scale;

void main() {
    vec2 pos = a_pos.xy * scale + offset;
    gl_Position = vec4(pos, a_pos.z, 1.0);
};