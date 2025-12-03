#version 330 core

layout (location = 0) in vec3 a_pos;

uniform vec2 u_offset;
uniform float u_scale;

void main() {
    vec2 pos = a_pos.xy * u_scale + u_offset;
    gl_Position = vec4(pos, a_pos.z, 1.0);
}